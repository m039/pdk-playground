/*
  PMS150C Voltage Meter (1-Bit ADC via Comparator) + I2C Slave.
  The Padauk MCU acts as an I2C slave at address 0x2A; Arduino Nano is the master.

  Register map:
    Reg 0: VDD (units of 100 mV; 0 = out of range)
    Reg 1: VPA4 (units of 100 mV; 0 = out of range)
    Reg 2..7: 6-byte Unique ID (defined at compile time)

  Pins:
    PA0 = SCL (input, driven by master clock)
    PA5 = SDA (open-drain, bidirectionally driven with output latch fixed at 0)
    PA4 = Analog comparator input for VPA4 measurement

  Pull-ups: External 4.7k resistors to bus VDD (internal weak PAPH pull-ups as backup).
  F_CPU: 16 MHz required to poll edges reliably with 50 kHz I2C clock.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define I2C_ADDR 0x2A
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

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
static uint16_t tick;       /* ADC measurement scheduler tick counter */
static uint8_t reg[2 + (sizeof(ID) / sizeof(ID[0]))];

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

    /* PADIER is write-only: configure digital input buffers.
       PA0 (SCL) and PA5 (SDA) are enabled as digital inputs.
       PA4 digital buffer is disabled to prevent leakage into analog comparator. */
    PADIER = SCL_BIT | SDA_BIT;

    /* Enable weak internal pull-ups as backup for external 4.7k resistors */
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    last = PA;

    /* Initialize register bank with Unique ID */
    for (uint8_t i = 0; i < sizeof(ID) / sizeof(ID[0]); i++)
    {
        reg[2 + i] = ID[i];
    }

    while (1)
    {
        /* Perform ADC measurement periodically when bus is idle.
           During measurement (~0.4 ms) slave does not service I2C;
           master will retry if request coincides. */
        if (tick++ == 0 && state == ST_IDLE)
        {
            int8_t nvdd = get_nvdd();
            int8_t nvpa4 = get_nvpa4();
            reg[0] = get_vdd(nvdd);
            reg[1] = get_vpa4(nvpa4, nvdd);
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
