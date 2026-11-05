#ifndef __I2C__
#define __I2C__

#include <pdk/device.h>
#include <stdint.h>
#include "delay.h"

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

/* --- Low-level open-drain SDA control (PA latch is kept 0 permanently) --- */
#define SDA_LOW()                                        \
    do                                                   \
    {                                                    \
        pause_sda = 1;                                   \
        PAC |= SDA_BIT; /* Output mode: pin pulls low */ \
    } while (0)

#define SDA_HIGH()                                                                        \
    do                                                                                    \
    {                                                                                     \
        pause_sda = 1;                                                                    \
        PAC &= (uint8_t)~SDA_BIT; /* Input mode: line pulled high by external resistor */ \
    } while (0)

#define SDA_RELEASE()                                          \
    do                                                         \
    {                                                          \
        pause_sda = 0;                                         \
        PAC &= (uint8_t)~SDA_BIT; /* Release line to master */ \
    } while (0)

#define SEND_NEXT_BIT()  \
    do                   \
    {                    \
        if (data & 0x80) \
            SDA_HIGH();  \
        else             \
            SDA_LOW();   \
        data <<= 1;      \
    } while (0)

static uint8_t state;
static uint8_t data;      /* Shift register for address/data */
static uint8_t ptr;       /* Current register index being read */
static uint8_t len;       /* Bit counter */
static uint8_t pause_sda; /* 1 = slave drives SDA, ignore pin change events */
static uint8_t i2c_rw;    /* Read/write bit sampled from address phase */
static uint8_t last;      /* Previous state of Port A pins */
static uint8_t *reg_data;
static uint8_t reg_length;

inline uint8_t i2c_is_idle()
{
    return !state;
}

inline void i2c_irq_handler() {

}

inline void i2c_init(uint8_t *data, uint8_t length)
{
    reg_data = data;
    reg_length = length;

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

    INTRQ = 0;
    __engint();                     // Enable global interrupts
}

static uint8_t scl_current;
static uint8_t sda_current;

inline void i2c_process()
{
    uint8_t current = PA;
    uint8_t tmp = last ^ current;

    if (tmp & SCL_BIT) /* scl_change */
    {
        scl_current = current & SCL_BIT;

        if (scl_current) /* SCL rising edge (0 -> 1): sample SDA */
        {
            sda_current = current & SDA_BIT;

            if (state == ST_ADDR)
            {
                len++;
                data <<= 1;
                if (sda_current)
                    data |= 1;

                if (len == 8)
                {
                    i2c_rw = sda_current;
                    state = ST_ACK;
                }
                else if (len == 7 && data != I2C_ADDR)
                {
                    state = ST_IDLE; /* Not our address; return to idle */
                }
            }
            else if (state == ST_ACK)
            {
                if (i2c_rw)
                {
                    state = ST_READ;
                    data = reg_data[ptr];
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
                if (len == 8)        /* 8th bit sampled by master */
                    state = ST_RACK; /* Await master ACK / NAK */
            }
            else if (state == ST_WRITE)
            {
                len++;
                data <<= 1;
                if (sda_current)
                    data |= 1;

                if (len == 8)
                {
                    /* Data received is the requested register pointer */
                    ptr = data;
                    state = ST_ACK;
                }
            }
            else if (state == ST_RACK)
            {
                if (sda_current) /* NAK: master finished reading */
                {
                    state = ST_IDLE;
                }
                else /* ACK: master requests sequential byte */
                {
                    if (ptr >= reg_length)
                    {
                        data = 0xFF;
                    }
                    else
                    {
                        data = reg_data[ptr++];
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
            } else {
                SDA_RELEASE();
            }
        }

        last = current;
    }
    else
    {
        scl_current = current & SCL_BIT;

        if ((tmp & SDA_BIT) /* sda _change */ && !pause_sda && scl_current)
        {
            sda_current = current & SDA_BIT;

            if (sda_current)
            {
                /* SDA rising edge while SCL high (0 -> 1): STOP condition */
                state = ST_IDLE;
            }
            else
            {
                /* SDA falling edge while SCL high (1 -> 0): START / REPEATED START condition */
                if (state)
                {
                    ptr = 0; /* Reset register pointer on fresh START */
                }

                state = ST_ADDR;
                len = 0;
                data = 0;
            }

        }

        last = current;
    }
}

#endif
