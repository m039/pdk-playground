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

const uint8_t ID[6] =  { 0x22, 0x55, 0x33, 0x11, 0x33, 0x53};

enum
{
    ST_IDLE, /* шина свободна */
    ST_ADDR, /* принимаем 7 бит адреса */
    ST_ACK,
    ST_READDATA, /* выдаём байт мастеру */
    ST_WRITE,
    ST_RACK /* слот ACK мастера: ACK = продолжаем, NAK = конец */
};

static uint8_t state;
static uint8_t data;      /* сдвиговый регистр адреса/байта */
static uint8_t ptr;       /* индекс отдаваемого байта */
static uint8_t len;       /* счётчик бит */
static uint8_t pause_sda; /* 1 = линию ведёт слейв, перепады игнорируем */
static uint8_t i2c_rw;    /* бит чтения/записи из адресного слота */
static uint8_t last;      /* предыдущее состояние PA */
static uint8_t vdd_v, pa4_v;
static uint16_t tick;
static uint8_t reg;

/* --- низкоуровневые операции с SDA (открытый сток, защёлка PA = 0) --- */
#define SDA_LOW()       \
    do                  \
    {                   \
        pause_sda = 1;  \
        PAC |= SDA_BIT; \
    } while (0)
#define SDA_HIGH()                \
    do                            \
    {                             \
        pause_sda = 1;            \
        PAC &= (uint8_t)~SDA_BIT; \
    } while (0)
#define SDA_RELEASE()             \
    do                            \
    {                             \
        pause_sda = 0;            \
        PAC &= (uint8_t)~SDA_BIT; \
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
        /* Измерение по переполнению тика, только пока шина свободна:
           во время него (~0.4 мс) слейв слепнет, мастер ретраит. */
        if (tick++ == 0 && state == ST_IDLE)
        {
            int8_t nvdd = get_nvdd();
            int8_t nvpa4 = get_nvpa4();
            vdd_v = get_vdd(nvdd); /* 0 = вне шкалы */
            pa4_v = get_vpa4(nvpa4, nvdd);
        }

        uint8_t current = PA;
        uint8_t tmp = last ^ current;
        uint8_t scl_change = tmp & SCL_BIT;
        uint8_t sda_change = tmp & SDA_BIT;
        last = current;

        if (scl_change)
        {
            if (current & SCL_BIT) /* фронт 0->1: чтение SDA */
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
                        state = ST_IDLE;
                    }
                }
                else if (state == ST_ACK)
                {
                    if (i2c_rw)
                    {
                        state = ST_READDATA;
                        ptr = 0;
                        if (reg == 0)
                        {
                            data = vdd_v;
                        }
                        else if (reg == 1)
                        {
                            data = pa4_v;
                        }
                        else if (reg == 2) {
                            data = ID[0];
                        }
                        else
                        {
                            data = 0;
                        }
                        len = 0;
                    }
                    else
                    {
                        state = ST_WRITE;
                        len = 0;
                        data = 0;
                    }
                }
                else if (state == ST_READDATA)
                {
                    if (len >= 8)        /* 8-й бит мастер только что сэмплировал */
                        state = ST_RACK; /* теперь его очередь отвечать */
                }
                else if (state == ST_WRITE)
                {
                    len++;
                    data <<= 1;
                    if (current & SDA_BIT)
                        data |= 1;

                    if (len >= 8)
                    {
                        reg = data;
                        state = ST_ACK;
                    }
                }
                else if (state == ST_RACK)
                {
                    if (current & SDA_BIT)   /* NAK — конец чтения */
                    {
                        state = ST_IDLE;
                    }
                    else                     /* ACK — мастер хочет ещё байт */
                    {
                        if ((reg == 2) && (ptr < 6)) {
                            data = ID[ptr++];
                        }
                        len = 0;
                        state = ST_READDATA;
                    }
                }
            }
            else /* спад 1->0: запись SDA */
            {
                if (state == ST_ACK)
                {
                    SDA_LOW();
                }
                else if (state == ST_WRITE)
                {
                    SDA_RELEASE();
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

        if (sda_change && !pause_sda && (current & SCL_BIT) && !scl_change)
        {
            if (current & SDA_BIT)
            {
                // 0 -> 1
                state = ST_IDLE; /* STOP */
            }
            else
            {
                // 1 -> 0
                state = ST_ADDR; /* START (в т.ч. повторный) */
                len = 0;
                ptr = 0;
                data = 0;
            }
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
