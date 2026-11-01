/*
  Измеритель напряжения PMS150C (1-bit ADC) + I2C-слейв с адресацией регистров.
  Падаук — слейв на адресе 0x2A, Нана — мастер.

  Регистры:
    Регистр 0: VDD (1 байт, единица = 100 мВ, 0 = вне шкалы)
    Регистр 1: PA4 (1 байт, единица = 100 мВ, 0 = вне шкалы)
    Регистр 2: 8-байтовый уникальный ID (задается при компиляции через UID_B0..UID_B7)

  Протокол мастера:
    Запись:  START -> [0x2A+W] -> [REG] -> STOP (или REPEATED START)
    Чтение:  START -> [0x2A+R] -> [BYTE 0] -> ... -> [BYTE N] -> STOP

  Ноги: PA0 = SCL (вход, такт ведёт Нана), PA5 = SDA (open-drain).
  Подтяжки внешние 4.7к на общий уровень питания шины (слабые PAPH — резерв).
  F_CPU = 16 МГц / 8 МГц.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define I2C_ADDR 0x2A
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

#ifndef UID_B0
#define UID_B0 0x50
#define UID_B1 0x44
#define UID_B2 0x4B
#define UID_B3 0x15
#define UID_B4 0x0C
#define UID_B5 0xA1
#define UID_B6 0xB2
#define UID_B7 0xC3
#endif

enum
{
    ST_IDLE,        /* 0: шина свободна */
    ST_ADDR,        /* 1: приём 7 бит адреса слейва */
    ST_OP,          /* 2: принят бит R/W, ждём слот ACK */
    ST_IACK1,       /* 3: спад: прижать SDA (ACK адреса) */
    ST_IACK2,       /* 4: спад: отпустить ACK и перейти к приёму/передаче */
    ST_WRITE_REG,   /* 5: мастер передаёт байт регистра */
    ST_WACK1,       /* 6: спад: прижать SDA (ACK регистра) */
    ST_WACK2,       /* 7: спад: отпустить SDA (конец записи регистра) */
    ST_READDATA,    /* 8: выдаём байт мастеру */
    ST_RACK         /* 9: слот ACK мастера */
};

static uint8_t state;
static uint8_t data;
static uint8_t len;
static uint8_t reg_idx;
static uint8_t byte_offset;
static uint8_t pause_sda;
static uint8_t i2c_rw;
static uint8_t last;
static uint8_t timeout;
static uint8_t tick_div;
static uint8_t vdd_v, pa4_v;
static uint16_t tick;

/* --- низкоуровневые операции с SDA (открытый сток, защёлка PA = 0) --- */
#define SDA_LOW()     do { pause_sda = 1; PAC |= SDA_BIT; } while(0)
#define SDA_HIGH()    do { pause_sda = 1; PAC &= (uint8_t)~SDA_BIT; } while(0)
#define SDA_RELEASE() do { pause_sda = 0; PAC &= (uint8_t)~SDA_BIT; } while(0)

#define SEND_NEXT_BIT() do { \
    if (data & 0x80) SDA_HIGH(); \
    else SDA_LOW(); \
    data <<= 1; \
} while(0)

static uint8_t load_reg_byte(void)
{
    if (reg_idx == 0) return vdd_v;
    if (reg_idx == 1) return pa4_v;
    switch (byte_offset++)
    {
        case 0: return UID_B0;
        case 1: return UID_B1;
        case 2: return UID_B2;
        case 3: return UID_B3;
        case 4: return UID_B4;
        case 5: return UID_B5;
        case 6: return UID_B6;
        case 7: return UID_B7;
    }
    return 0xFF;
}

static void do_measurement(void)
{
    int8_t nvdd = get_nvdd();
    int8_t nvpa4 = get_nvpa4();
    vdd_v = get_vdd(nvdd);
    pa4_v = get_vpa4(nvpa4, vdd_v);
}

void main(void)
{
    PA &= (uint8_t)~(SDA_BIT | SCL_BIT);  /* защёлки = 0 */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT); /* обе линии отпущены */

    /* PA0/PA5 — цифровые входы, PA4 отключён (компаратор) */
    PADIER = SCL_BIT | SDA_BIT;
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    reg_idx = 0;
    byte_offset = 0;
    pause_sda = 0;
    timeout = 0;
    tick_div = 0;
    tick = 0;
    last = PA;

    /* Первоначальный замер при включении */
    do_measurement();

    while (1)
    {
        uint8_t current = PA;
        uint8_t tmp = last ^ current;
        uint8_t scl_change = tmp & SCL_BIT;
        uint8_t sda_change = tmp & SDA_BIT;
        last = current;

        if (scl_change)
        {
            timeout = 0;

            if (PA & SCL_BIT) /* Фронт 0->1: сэмпл с SDA */
            {
                if (state == ST_ADDR)
                {
                    data <<= 1;
                    if (PA & SDA_BIT)
                        data |= 1;
                    if (++len >= 7)
                    {
                        state = (data == I2C_ADDR) ? ST_OP : ST_IDLE;
                    }
                }
                else if (state == ST_OP)
                {
                    i2c_rw = PA & SDA_BIT;
                    state = ST_IACK1;
                }
                else if (state == ST_IACK1)
                {
                    state = ST_IACK2;
                }
                else if (state == ST_WRITE_REG)
                {
                    data <<= 1;
                    if (PA & SDA_BIT)
                        data |= 1;
                    if (++len >= 8)
                    {
                        reg_idx = data;
                        state = ST_WACK1;
                    }
                }
                else if (state == ST_WACK1)
                {
                    state = ST_WACK2;
                }
                else if (state == ST_READDATA)
                {
                    if (len >= 8)
                    {
                        data = load_reg_byte();
                        len = 0;
                        state = ST_RACK;
                    }
                }
                else if (state == ST_RACK)
                {
                    state = (PA & SDA_BIT) ? ST_IDLE : ST_READDATA;
                }
            }
            else /* Спад 1->0: управление SDA */
            {
                if (state == ST_IACK1 || state == ST_WACK1)
                {
                    SDA_LOW(); /* ACK на адрес или на байт регистра */
                }
                else if (state == ST_IACK2)
                {
                    SDA_RELEASE();
                    if (i2c_rw)
                    {
                        state = ST_READDATA;
                        len = 1;
                        byte_offset = 0;
                        data = load_reg_byte();
                        SEND_NEXT_BIT();
                    }
                    else
                    {
                        state = ST_WRITE_REG;
                        data = 0;
                        len = 0;
                    }
                }
                else if (state == ST_WACK2)
                {
                    SDA_RELEASE();
                    state = ST_IDLE;
                }
                else if (state == ST_READDATA)
                {
                    SEND_NEXT_BIT();
                    len++;
                }
                else if (state == ST_RACK)
                {
                    SDA_RELEASE();
                }
            }
        }
        else if (state != ST_IDLE)
        {
            /* Watchdog: если SCL не меняется при активной транзакции — сброс шины в ST_IDLE */
            if (++timeout == 0)
            {
                state = ST_IDLE;
                SDA_RELEASE();
            }
        }

        /* START / STOP условия: перепад SDA при стабильно высоком SCL (только от мастера) */
        if (sda_change && !pause_sda && (PA & SCL_BIT) && !scl_change)
        {
            if (PA & SDA_BIT)
            {
                state = ST_IDLE; /* STOP */
                SDA_RELEASE();
            }
            else
            {
                state = ST_ADDR; /* START или REPEATED START */
                len = 0;
                data = 0;
                byte_offset = 0;
                SDA_RELEASE();
            }
        }

        /* Фоновое измерение напряжений: только пока шина полностью свободна (ST_IDLE) */
        if (++tick == 0 && state == ST_IDLE)
        {
            if (++tick_div >= 30) /* Замер раз в ~2-3 секунды */
            {
                if ((PA & (SCL_BIT | SDA_BIT)) == (SCL_BIT | SDA_BIT))
                {
                    tick_div = 0;
                    do_measurement();
                }
            }
        }
    }
}

unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
    return 0;
}
