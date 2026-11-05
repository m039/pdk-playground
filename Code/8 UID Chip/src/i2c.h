#ifndef __I2C__
#define __I2C__

#include <pdk/device.h>
#include <stdint.h>

#define I2C_ADDR 0x2B
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

#define SDA_LOW()     do { PAC |= SDA_BIT; } while (0)
#define SDA_RELEASE() do { PAC &= (uint8_t)~SDA_BIT; } while (0)

/* Timeout ~30 ms at 8 MHz */
#define I2C_TIMEOUT 60000U

/* Wait for SCL to fall to 0. Detects STOP condition (SDA rising while SCL is high). */
#define WAIT_SCL_LOW(ok)                                \
    do {                                                \
        uint16_t _t = 0;                                \
        uint8_t _prev = PA & SDA_BIT;                   \
        ok = 0;                                         \
        while (PA & SCL_BIT) {                          \
            uint8_t _cur = PA & SDA_BIT;                \
            if (!_prev && _cur) { ok = 0; break; }      /* STOP condition */ \
            _prev = _cur;                               \
            if (++_t >= I2C_TIMEOUT) { ok = 0; break; } /* Timeout */ \
        }                                               \
        if (PA & SCL_BIT) break;                        \
        ok = 1;                                         \
    } while (0)

/* Wait for SCL to rise to 1. */
#define WAIT_SCL_HIGH(ok)                               \
    do {                                                \
        uint16_t _t = 0;                                \
        ok = 0;                                         \
        while (!(PA & SCL_BIT)) {                       \
            if (++_t >= I2C_TIMEOUT) { ok = 0; break; } \
        }                                               \
        if (!(PA & SCL_BIT)) break;                     \
        ok = 1;                                         \
    } while (0)

static uint8_t *reg_data;
static uint8_t reg_length;
static uint8_t ptr;

inline void i2c_init(uint8_t *data, uint8_t length)
{
    reg_data = data;
    reg_length = length;
    ptr = 0;

    PA &= (uint8_t)~(SDA_BIT | SCL_BIT);  /* Output latches set to 0 */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT); /* Input mode (released) */

    PADIER = SCL_BIT | SDA_BIT;           /* Wake-up on SCL/SDA */
    PBDIER = 0x00;
    MISC |= MISC_FAST_WAKEUP_ENABLE;
    PAPH = SCL_BIT | SDA_BIT;             /* Internal pull-ups */
}

/* Receives 8 bits from master */
static uint8_t i2c_read_byte(uint8_t *out)
{
    uint8_t byte = 0;
    uint8_t ok;

    for (uint8_t i = 0; i < 8; i++)
    {
        WAIT_SCL_LOW(ok);
        if (!ok) return 0;

        WAIT_SCL_HIGH(ok);
        if (!ok) return 0;

        byte = (byte << 1) | ((PA & SDA_BIT) ? 1 : 0);
    }

    *out = byte;
    return 1;
}

/* Sends ACK to master */
static uint8_t i2c_send_ack(void)
{
    uint8_t ok;

    WAIT_SCL_LOW(ok);
    if (!ok) return 0;

    SDA_LOW(); /* Pull SDA low for ACK */

    WAIT_SCL_HIGH(ok);
    if (!ok) { SDA_RELEASE(); return 0; }

    WAIT_SCL_LOW(ok);
    SDA_RELEASE(); /* Release SDA */

    return ok;
}

/* Transmits 8 bits to master and reads master ACK/NAK */
static uint8_t i2c_write_byte(uint8_t byte, uint8_t *nak)
{
    uint8_t ok;

    for (uint8_t i = 0; i < 8; i++)
    {
        WAIT_SCL_LOW(ok);
        if (!ok) { SDA_RELEASE(); return 0; }

        if (byte & 0x80)
            SDA_RELEASE();
        else
            SDA_LOW();

        byte <<= 1;

        WAIT_SCL_HIGH(ok);
        if (!ok) { SDA_RELEASE(); return 0; }
    }

    /* 9th clock: release SDA and sample master ACK (0) or NAK (1) */
    WAIT_SCL_LOW(ok);
    SDA_RELEASE();
    if (!ok) return 0;

    WAIT_SCL_HIGH(ok);
    if (!ok) return 0;

    *nak = (PA & SDA_BIT) ? 1 : 0;

    WAIT_SCL_LOW(ok);
    return ok;
}

/* Main transaction handler called when START condition is detected */
inline void i2c_process(void)
{
    uint8_t byte;
    uint8_t ok;

    while (1)
    {
        /* 1. Receive 7-bit Address + R/W bit */
        if (!i2c_read_byte(&byte))
            return;

        uint8_t addr = byte >> 1;
        uint8_t rw = byte & 1;

        if (addr != I2C_ADDR)
            return; /* Not our address */

        /* 2. Address matches -> send ACK */
        if (!i2c_send_ack())
            return;

        if (rw == 0)
        {
            /* Master WRITE: receive target register pointer */
            if (!i2c_read_byte(&byte))
                return;

            ptr = byte;

            if (!i2c_send_ack())
                return;

            /* After ACK of register write: wait for Repeated START or STOP */
            WAIT_SCL_HIGH(ok);
            if (!ok) return;

            /* Monitor while SCL is HIGH:
               - If SDA goes LOW: REPEATED START! Loop to read new address!
               - If SDA goes HIGH: STOP! Return to idle. */
            uint16_t t = 0;
            while (PA & SCL_BIT)
            {
                if (!(PA & SDA_BIT))
                {
                    /* REPEATED START detected */
                    break;
                }
                if (++t >= I2C_TIMEOUT)
                    return;
            }

            continue;
        }
        else
        {
            /* Master READ: stream data bytes */
            while (1)
            {
                uint8_t val = (ptr < reg_length) ? reg_data[ptr++] : 0xFF;
                uint8_t nak;

                if (!i2c_write_byte(val, &nak))
                    return;

                if (nak)
                {
                    /* Master sent NAK: transmission complete */
                    if (ptr >= reg_length)
                        ptr = 0;
                    return;
                }
            }
        }
    }
}

#endif
