
/*
  PMS150C I2C Unique ID (UID) Chip Emulator

  Emulates a dedicated I2C slave (address 0x2B) that returns a 6-byte Unique ID (UID)
  from register 0x00.

  Hardware & Pinout (PMS150C / PFS154):
    - PA0 (Pin 1): SCL (requires 4.7 kΩ external pull-up to VCC)
    - PA5 (Pin 6): SDA (requires 4.7 kΩ external pull-up to VCC)
    - VDD (Pin 4): 2.4V – 5.5V DC supply
    - GND (Pin 2): Ground

  Power Management:
    - Enters ultra-low-power deep sleep ('stopsys') after 65536 idle passes on the bus.
    - Switches clock to ILRC and turns off IHRC before sleeping (< 0.5 µA standby current).
    - Resumes instantly upon SCL/SDA pin-change (PADIER) and restores 8 MHz IHRC (clkmd_old).
*/

#include <pdk/device.h>
#include <stdint.h>
#include "auto_sysclock.h"

#define I2C_ADDR 0x2B
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

/* 6-byte Unique ID payload returned on I2C read requests */
const uint8_t ID[6] = { 0x22, 0x55, 0x33, 0x11, 0x33, 0x53 };

/* I2C Slave state machine states */
enum
{
    ST_IDLE,    /* Bus idle / awaiting START condition */
    ST_ADDR,    /* Receiving 7-bit slave address + R/W bit */
    ST_ACK,     /* Acknowledging address or received register pointer */
    ST_READ,    /* Transmitting data byte to master */
    ST_WRITE,   /* Receiving register pointer byte from master */
    ST_RACK     /* Master ACK/NAK slot: ACK = send next byte, NAK = stop */
};

static uint8_t state;
static uint8_t data;        /* Shift register for address/data */
static uint8_t ptr;         /* Current register index being read */
static uint8_t len;         /* Bit counter */
static uint8_t pause_sda;   /* 1 = slave drives SDA, ignore pin change events */
static uint8_t i2c_rw;      /* Read/write bit sampled from address phase */
static uint8_t last;        /* Previous state of Port A pins */
static uint16_t tick;      
#define reg ID

/* --- Low-level open-drain SDA control (PA latch is kept 0 permanently) --- */
#define SDA_LOW() \
    do \
    { \
        pause_sda = 1; \
        PAC |= SDA_BIT; /* Output mode: pin pulls low */ \
    } while (0)

#define SDA_HIGH() \
    do \
    { \
        pause_sda = 1; \
        PAC &= (uint8_t)~SDA_BIT; /* Input mode: line pulled high by external resistor */ \
    } while (0)

#define SDA_RELEASE() \
    do \
    { \
        pause_sda = 0; \
        PAC &= (uint8_t)~SDA_BIT; /* Release line to master */ \
    } while (0)

#define SEND_NEXT_BIT() \
    do \
    { \
        if (data & 0x80) \
            SDA_HIGH(); \
        else \
            SDA_LOW(); \
        data <<= 1; \
    } while (0)

void main(void)
{
    PA &= (uint8_t)~(SDA_BIT | SCL_BIT);  /* Output latches set to 0 permanently */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT); /* Both lines released (input mode) */

    /* Wake-up from stopsys enabled ONLY for SCL (PA0) and SDA (PA5):
       any toggle on these pins wakes the CPU. All other pins must not. */
    PADIER = SCL_BIT | SDA_BIT;
    PBDIER = 0x00; /* Port B must not wake the MCU */

    /* Fast wake-up: reduces oscillator warm-up time upon wake-up */
    MISC |= MISC_FAST_WAKEUP_ENABLE;

    /* Enable weak internal pull-ups as backup for external 4.7k resistors */
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    last = PA;
    uint8_t clkmd_old = CLKMD;

    while (1)
    {
        if (state == ST_IDLE)
        {
            /* Stay awake for 65536 idle passes after the last bus activity,
               then enter deep sleep (stopsys). Clock is switched to ILRC and
               IHRC is disabled for sub-microamp standby. Upon wake-up by
               SCL/SDA toggle, IHRC is restored to clkmd_old. */
            if (++tick == 0)
            {
                CLKMD = 0xF4;
                CLKMD &= ~CLKMD_ENABLE_IHRC;
                __asm stopsys __endasm;
                __asm nop __endasm;
                CLKMD = clkmd_old;
            }
        }
        else
        {
            tick = 0; /* Bus active: restart the idle countdown */
        }

        uint8_t current = PA;
        uint8_t tmp = last ^ current;
        uint8_t scl_change = tmp & SCL_BIT;
        uint8_t sda_change = tmp & SDA_BIT;
        last = current;

        if (scl_change)
        {
            if (current & SCL_BIT) /* SCL rising edge (0 -> 1): sample SDA */
            {
                if (state == ST_ADDR)
                {
                    len++;
                    data <<= 1;
                    if (current & SDA_BIT)
                        data |= 1;

                    if (len >= 8)
                    {
                        i2c_rw = current & SDA_BIT;
                        state = ST_ACK;
                    }
                    else if (len >= 7 && data != I2C_ADDR)
                    {
                        state = ST_IDLE; /* Not our address; return to idle */
                    }
                }
                else if (state == ST_ACK)
                {
                    if (i2c_rw)
                    {
                        state = ST_READ;
                        data = reg[ptr];
                        len = 0;
                    }
                    else
                    {
                        state = ST_WRITE;
                        len = 0;
                        data = 0;
                    }
                }
                else if (state == ST_READ)
                {
                    if (len >= 8)        /* 8th bit sampled by master */
                        state = ST_RACK; /* Await master ACK / NAK */
                }
                else if (state == ST_WRITE)
                {
                    len++;
                    data <<= 1;
                    if (current & SDA_BIT)
                        data |= 1;

                    if (len >= 8)
                    {
                        /* Data received is the requested register pointer */
                        ptr = data;
                        state = ST_ACK;
                    }
                }
                else if (state == ST_RACK)
                {
                    if (current & SDA_BIT)   /* NAK: master finished reading */
                    {
                        state = ST_IDLE;
                    }
                    else                     /* ACK: master requests sequential byte */
                    {
                        if (ptr >= sizeof(reg) / sizeof(reg[0]))
                        {
                            data = 0xFF;
                        }
                        else
                        {
                            data = reg[ptr++];
                        }
                        len = 0;
                        state = ST_READ;
                    }
                }
            }
            else /* SCL falling edge (1 -> 0): drive SDA */
            {
                if (state == ST_ACK)
                {
                    SDA_LOW(); /* Drive ACK (pull SDA low) */
                }
                else if (state == ST_WRITE)
                {
                    SDA_RELEASE(); /* Release SDA after register ACK */
                }
                else if (state == ST_READ)
                {
                    SEND_NEXT_BIT();
                    len++;
                }
                else if (state == ST_RACK)
                {
                    SDA_RELEASE(); /* Release SDA for master ACK slot */
                }
            }
        }

        /* START / STOP condition detection: SDA changes while SCL is high */
        if (sda_change && !pause_sda && (current & SCL_BIT) && !scl_change)
        {
            if (current & SDA_BIT)
            {
                /* SDA rising edge while SCL high (0 -> 1): STOP condition */
                state = ST_IDLE;
            }
            else
            {
                /* SDA falling edge while SCL high (1 -> 0): START / REPEATED START condition */
                if (state == ST_IDLE)
                {
                    ptr = 0; /* Reset register pointer on fresh START */
                }

                state = ST_ADDR;
                len = 0;
                data = 0;
            }
        }
    }
}

/*
  Startup code - setup and calibrate internal system clock.
*/
unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
    return 0;
}
