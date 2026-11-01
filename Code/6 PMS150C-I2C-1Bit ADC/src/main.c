/*
  Измеритель напряжения PMS150C (1-bit ADC) + I2C-слейв "только чтение".
  Падаук — слейв на адресе 0x2A, Нана — мастер.
  Нана делает Wire.requestFrom(0x2A, 2) и получает два байта:
    байт 0 — VDD, байт 1 — PA4;
  единица = 100 мВ, 0 = "вне шкалы" (лестница кончилась).

  Ноги: PA0 = SCL (вход, такт ведёт Нана), PA5 = SDA (open-drain).
  Подтяжки внешние 4.7к на общий уровень питания шины (слабые PAPH — резерв).
  F_CPU = 16 МГц обязателен: поллинг фронта при 50 кГц живёт на латентности цикла.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define I2C_ADDR 0x2A
#define PIN_SCL 0
#define PIN_SDA 5
#define SCL_BIT ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT ((uint8_t)(1 << PIN_SDA))

enum
{
    ST_IDLE,     /* шина свободна */
    ST_ADDR,     /* принимаем 7 бит адреса */
    ST_OP,       /* принят бит R/W, ждём слот ACK */
    ST_IACK1,    /* спад: прижать SDA (ACK) */
    ST_IACK2,    /* спад: отпустить и выдать первый бит данных */
    ST_READDATA, /* выдаём байт мастеру */
    ST_RACK      /* слот ACK мастера: ACK = продолжаем, NAK = конец */
};

static uint8_t state;
static uint8_t data;      /* сдвиговый регистр адреса/байта */
static uint8_t len;       /* счётчик бит */
static uint8_t ptr;       /* индекс отдаваемого байта */
static uint8_t pause_sda; /* 1 = линию ведёт слейв, перепады игнорируем */
static uint8_t i2c_rw;    /* бит чтения/записи из адресного слота */
static uint8_t last;      /* предыдущее состояние PA */
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

void main(void)
{
    PA &= (uint8_t)~(SDA_BIT | SCL_BIT);  /* защёлки = 0 навсегда */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT); /* обе линии отпущены */

    /* write-only: пишем целиком. PA0/PA5 — цифровые входы, PA4 отключён
       (аналоговый вход компаратора, буфер ноги давит утечками). */
    PADIER = SCL_BIT | SDA_BIT;

    /* Слабые подтяжки — резерв к внешним 4.7к. */
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    last = PA;

    while (1)
    {
        uint8_t current = PA;
        uint8_t tmp = last ^ current;
        uint8_t scl_change = tmp & SCL_BIT;
        uint8_t sda_change = tmp & SDA_BIT;
        last = current;

        if (scl_change)
        {
            if (PA & SCL_BIT) /* фронт 0->1: сэмпл с SDA */
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
                else if (state == ST_READDATA)
                {
                    if (len >= 8)
                    {
                        ptr++;
                        data = (ptr == 1) ? pa4_v : 0xFF;
                        len = 0;
                        state = ST_RACK;
                    }
                }
                else if (state == ST_RACK)
                {
                    state = (PA & SDA_BIT) ? ST_IDLE : ST_READDATA;
                }
            }
            else /* спад 1->0: управляем SDA */
            {
                if (state == ST_IACK1)
                {
                    if (i2c_rw)
                        SDA_LOW(); /* ACK только на чтение */
                }
                else if (state == ST_IACK2)
                {
                    if (!i2c_rw)
                    {
                        state = ST_IDLE; /* запись NAK-нута, ждём STOP */
                    }
                    else
                    {
                        SDA_RELEASE();
                        state = ST_READDATA;
                        ptr = 0;
                        len = 1;
                        data = vdd_v;
                        SEND_NEXT_BIT();
                    }
                }
                else if (state == ST_READDATA)
                {
                    SEND_NEXT_BIT();
                    len++;
                }
                else if (state == ST_RACK)
                {
                    SDA_RELEASE(); /* освобождаем линию к слоту ACK */
                }
            }
        }

        if (sda_change && !pause_sda && (PA & SCL_BIT) && !scl_change)
        {
            if (PA & SDA_BIT)
            {
                state = ST_IDLE; /* STOP */
            }
            else
            {
                state = ST_ADDR; /* START (в т.ч. повторный) */
                len = 0;
                ptr = 0;
                data = 0;
            }
        }

        /* Измерение по переполнению тика, только пока шина свободна:
           во время него (~0.4 мс) слейв слепнет, мастер ретраит. */
        if (++tick == 0 && state == ST_IDLE)
        {
            int8_t nvdd = get_nvdd();
            int8_t nvpa4 = get_nvpa4();
            vdd_v = get_vdd(nvdd); /* 0 = вне шкалы */
            pa4_v = get_vpa4(nvpa4, nvdd);
        }
    }
}

/*
  Startup code — настройка и калибровка системного клока.
*/
unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
    return 0;
}
