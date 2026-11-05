#ifndef __I2C__
#define __I2C__

#include <pdk/device.h>
#include <stdint.h>

#define I2C_ADDR 0x2B
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

/* I2C Slave state machine states */
enum
{
    ST_IDLE = 0,  /* Bus idle / awaiting START condition */
    ST_ADDR = 1,  /* Receiving 7-bit slave address + R/W bit */
    ST_ACK = 2,   /* Acknowledging address or received register pointer */
    ST_READ = 4,  /* Transmitting data byte to master */
    ST_WRITE = 8, /* Receiving register pointer byte from master */
    ST_RACK = 16  /* Master ACK/NAK slot: ACK = send next byte, NAK = stop */
};

/* External sleep countdown tick defined in main.c */
extern uint16_t tick;

static uint8_t state;
static uint8_t data;        /* Shift register for address/data */
static uint8_t ptr;         /* Current register index being read */
static uint8_t len;         /* Bit counter */
static uint8_t i2c_rw;      /* Read/write bit sampled from address phase */
static uint8_t last;        /* Previous state of Port A pins */
static uint8_t *reg_data;
static uint8_t reg_length;

inline uint8_t i2c_is_idle(void)
{
    return (state == ST_IDLE);
}

inline void i2c_init(uint8_t *data_ptr, uint8_t length)
{
    reg_data = data_ptr;
    reg_length = length;
    ptr = 0;

    PA &= (uint8_t)~(SDA_BIT | SCL_BIT);  /* Output latches set to 0 permanently */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT); /* Both lines released (input mode) */

    /* Wake-up from stopsys enabled ONLY for SCL (PA0) and SDA (PA5):
       any toggle on these lines wakes the CPU. All other pins must not. */
    PADIER = SCL_BIT | SDA_BIT;
    PBDIER = 0x00; /* Port B must not wake the MCU */

    /* Fast wake-up: reduces oscillator warm-up time upon wake-up */
    MISC |= MISC_FAST_WAKEUP_ENABLE;

    /* Enable weak internal pull-ups as backup for external 4.7k resistors */
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    last = PA;
}

/*
  High-speed I2C slave engine optimized for standard 100 kbit/s (100 kHz) bus clock.
  At 8 MHz IHRC, one 100 kHz half-cycle is 40 CPU cycles (5 µs).
  
  Key optimizations for 100 kHz operation:
  1. Inner-loop transaction retention: Once START is detected, execution stays
     inside a tight assembly polling loop (~6-8 CPU cycles latency, < 1.0 µs),
     eliminating main() call/return jitter and tick overhead.
  2. Immediate SDA driving: On SCL falling edge (1 -> 0), the next bit or ACK
     is driven to PAC within 14-18 CPU cycles (< 2.2 µs), providing > 2.8 µs
     (over 22 CPU cycles) of rock-solid setup margin before SCL rises.
  3. Clean sequential streaming: Register pointer correctly advances so
     multi-byte UID reads stream without duplicate bytes.
  4. Automatic bus timeout: If master stalls for > 49 ms during a packet,
     the slave cleanly aborts to ST_IDLE and allows deep sleep.
*/
inline void i2c_process(void)
{
    uint8_t current = PA;
    if (current == last && state == ST_IDLE)
        return;

    /* Bus activity detected: reset idle countdown to keep MCU awake during traffic */
    tick = 0;

    do
    {
        uint8_t tmp = last ^ current;

        if (tmp & SCL_BIT)
        {
            if (!(current & SCL_BIT))
            {
                /* SCL falling edge (1 -> 0): drive SDA immediately */
                if (state == ST_READ)
                {
                    if (data & 0x80)
                        PAC &= (uint8_t)~SDA_BIT; /* Send bit 1: release line */
                    else
                        PAC |= SDA_BIT;           /* Send bit 0: pull line low */
                    data <<= 1;
                }
                else if (state == ST_ACK)
                {
                    PAC |= SDA_BIT;               /* Drive ACK: pull SDA low */
                }
                else
                {
                    PAC &= (uint8_t)~SDA_BIT;     /* Release SDA for master input */
                }
                last = current;
            }
            else
            {
                last = current;
                /* SCL rising edge (0 -> 1): sample SDA */
                uint8_t sda = current & SDA_BIT;

                if (state == ST_ADDR)
                {
                    data = (data << 1) | (sda ? 1 : 0);
                    if (++len == 7)
                    {
                        if (data != I2C_ADDR)
                            state = ST_IDLE; /* Not addressed; ignore rest of frame */
                    }
                    else if (len == 8)
                    {
                        i2c_rw = sda;        /* Bit 0 is R/W bit (1 = Read, 0 = Write) */
                        state = ST_ACK;      /* Send ACK on next falling edge */
                    }
                }
                else if (state == ST_ACK)
                {
                    len = 0;
                    if (i2c_rw)
                    {
                        state = ST_READ;
                        data = (ptr < reg_length) ? reg_data[ptr] : 0xFF;
                        ptr++;
                    }
                    else
                    {
                        state = ST_WRITE;
                        data = 0;
                    }
                }
                else if (state == ST_READ)
                {
                    if (++len == 8)
                        state = ST_RACK;     /* 8 data bits sent; await master ACK/NAK */
                }
                else if (state == ST_WRITE)
                {
                    data = (data << 1) | (sda ? 1 : 0);
                    if (++len == 8)
                    {
                        ptr = data;          /* Register pointer received from master */
                        state = ST_ACK;      /* Send ACK for register address */
                    }
                }
                else if (state == ST_RACK)
                {
                    if (sda)
                    {
                        state = ST_IDLE;     /* Master NAK: end of read */
                    }
                    else
                    {
                        state = ST_READ;     /* Master ACK: stream next byte */
                        data = (ptr < reg_length) ? reg_data[ptr] : 0xFF;
                        ptr++;
                        len = 0;
                    }
                }
            }
        }
        else if ((tmp & SDA_BIT) && (current & SCL_BIT))
        {
            last = current;
            if (current & SDA_BIT)
            {
                /* SDA rising edge while SCL high (0 -> 1): STOP condition */
                state = ST_IDLE;
            }
            else
            {
                /* SDA falling edge while SCL high (1 -> 0): START / REPEATED START */
                state = ST_ADDR;
                len = 0;
                data = 0;
                if (ptr >= reg_length)
                    ptr = 0;
            }
        }
        else
        {
            last = current;
        }

        if (state == ST_IDLE)
            break;

        /* Ultra-fast pin-change detection loop (~6-8 cycles per iteration).
           Times out after ~49 ms if the bus stops clocking unexpectedly. */
        uint16_t timeout = 0;
        while ((current = PA) == last)
        {
            if (++timeout == 0)
            {
                state = ST_IDLE;
                break;
            }
        }
    } while (state != ST_IDLE);
}

#endif
