/*
  Измеритель напряжения PMS150C (1-bit ADC) + I2C-слейв "только чтение".

  Падаук — слейв на адресе 0x2A, Нана — мастер.
  Нана делает Wire.requestFrom(0x2A, 2) и получает два байта:
    байт 0 — VDD, байт 1 — PA4;
  единица = 100 мВ, 0 = "вне шкалы" (лестница кончилась).

  Структура слейва повторяет найденный в сети поллинговый пример для
  Padauk IDE (i2c_stm.c), портированный на SDCC/Free-PDK:
    - START/STOP детектируются по перепаду SDA при стабильном высоком SCL;
    - флаг pause_sda: пока слейв сам ведёт линию, свои перепады не
      принимаются за START/STOP;
    - первый бит каждого байта выставляется сразу на переходном спаде
      SCL — мастер сэмплирует его на ближайшем фронте (без сдвига на такт).

  Отличия от оригинала:
    - ноги SCL = PA0, SDA = PA5 (PA4 занята компаратором); добавлен PADIER;
    - запись от мастера не поддерживается: адресный слот с R/W=0 получает NAK;
    - вместо буфера 16 байт — два значения из ADC, после них 0xFF;
    - оригинал начинал выдачу с buf[1] (buf[0] пропускался) — здесь
      выдача начинается с vdd_v;
    - .ADJUST_IC заменён на _sdcc_external_startup
      (AUTO_INIT_SYSCLOCK / AUTO_CALIBRATE_SYSCLOCK);
    - DEBUG-вывод через serial убран: прерывание TM2 ломало бы поллинг шины.

  Ноги: PA0 = SCL (вход, такт ведёт Нана), PA5 = SDA (open-drain).
  Подтяжки внешние 4.7к на общий уровень питания шины (слабые PAPH — резерв).
  F_CPU = 16 МГц обязателен: поллинг фронта при 50 кГц живёт на латентности
  цикла. Clock stretching нет: во время измерения (~0.4 мс) слейв слепнет —
  мастер должен ретраить.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define I2C_ADDR  0x2A
#define PIN_SCL   0
#define PIN_SDA   5
#define SCL_BIT   ((uint8_t)(1 << PIN_SCL))
#define SDA_BIT   ((uint8_t)(1 << PIN_SDA))

enum {
    ST_IDLE,        /* шина свободна */
    ST_ADDR,        /* принимаем 7 бит адреса */
    ST_OP,          /* принят бит R/W, ждём слот ACK */
    ST_IACK1,       /* спад: прижать SDA (ACK) */
    ST_IACK2,       /* спад: отпустить и выдать первый бит данных */
    ST_READDATA,    /* выдаём байт мастеру */
    ST_RACK         /* слот ACK мастера: ACK = продолжаем, NAK = конец */
};

static uint8_t state;
static uint8_t data;        /* сдвиговый регистр адреса/байта */
static uint8_t len;         /* счётчик бит */
static uint8_t ptr;         /* индекс отдааваемого байта */
static uint8_t pause_sda;   /* 1 = линию ведёт слейв, перепады игнорируем */
static uint8_t i2c_rw;      /* бит чтения/записи из адресного слота */
static uint8_t last;        /* предыдущее состояние PA */
static uint8_t scl_change;
static uint8_t sda_change;

static uint8_t vdd_v, pa4_v;
static uint16_t tick;

/* --- низкоуровневые операции с SDA (открытый сток, защёлка PA = 0) --- */

static void clear_sda(void)     /* держать SDA низко, свои перепады игнорировать */
{
    pause_sda = 1;
    PAC |= SDA_BIT;
}

static void release_sda(void)   /* отпустить линию, снова слушать перепады */
{
    pause_sda = 0;
    PAC &= (uint8_t)~SDA_BIT;
}

static void sda_low(void)
{
    pause_sda = 1;
    PAC |= SDA_BIT;
}

static void sda_high(void)
{
    pause_sda = 1;
    PAC &= (uint8_t)~SDA_BIT;
}

/* Очередной байт выдачи: 0 = VDD, 1 = PA4, дальше 0xFF.
   Единица 100 мВ, 0 = вне шкалы. */
static uint8_t load_next(void)
{
    if (ptr == 0) return vdd_v;
    if (ptr == 1) return pa4_v;
    return 0xFF;                /* мастер обязан NAK-нуть и завершить */
}

static void check_scl(void)
{
    if (PA & SCL_BIT)           /* фронт 0->1: сэмпл с SDA */
    {
        switch (state)
        {
            case ST_ADDR:
                data = (uint8_t)((data << 1) | ((PA & SDA_BIT) ? 1 : 0));
                len++;
                if (len < 7)
                    break;
                if (data != I2C_ADDR)
                    state = ST_IDLE;    /* не наш адрес: молчим */
                else
                    state = ST_OP;      /* остался бит R/W */
                break;

            case ST_OP:
                i2c_rw = (PA & SDA_BIT) ? 1 : 0;
                state = ST_IACK1;
                break;

            case ST_IACK1:
                state = ST_IACK2;
                break;

            case ST_READDATA:
                if (len < 8)
                    break;
                ptr++;                  /* байт выдан: готовим следующий */
                data = load_next();
                len = 0;
                state = ST_RACK;
                break;

            case ST_RACK:               /* слот ACK мастера */
                if (PA & SDA_BIT)
                    state = ST_IDLE;    /* NAK: мастер закончил */
                else
                    state = ST_READDATA;
                break;

            default:
                break;
        }
    }
    else                        /* спад 1->0: управляем SDA */
    {
        switch (state)
        {
            case ST_IACK1:
                if (i2c_rw)
                    clear_sda();        /* ACK только на чтение */
                /* запись не поддерживается: линию не прижимаем —
                   мастер увидит NAK в адресном слоте */
                break;

            case ST_IACK2:
                if (i2c_rw == 0)
                {
                    state = ST_IDLE;    /* запись NAK-нута, ждём STOP */
                    break;
                }
                release_sda();
                state = ST_READDATA;
                ptr = 0;
                len = 1;                /* первый бит УЖЕ выдан ниже — считаем его */
                /* первый бит — немедленно на этом спаде */
                data = load_next();
                if (data & 0x80) sda_high();
                else             sda_low();
                data = (uint8_t)(data << 1);
                break;

            case ST_READDATA:
                if (data & 0x80) sda_high();
                else             sda_low();
                data = (uint8_t)(data << 1);
                len++;
                break;

            case ST_RACK:
                release_sda();          /* освобождаем линию к слоту ACK */
                break;

            default:
                break;
        }
    }
}

static void check_sda(void)
{
    /* пока линию ведёт сам слейв, свои перепады за START/STOP не считаем */
    if (pause_sda)
        return;

    if ((PA & SCL_BIT) && !scl_change)
    {
        if (PA & SDA_BIT)
        {
            state = ST_IDLE;            /* STOP */
        }
        else
        {
            state = ST_ADDR;            /* START (в т.ч. повторный) */
            len = 0;
            ptr = 0;
            data = 0;
        }
    }
}

static void i2c_init(void)
{
    PA  &= (uint8_t)~(SDA_BIT | SCL_BIT);   /* защёлки = 0 навсегда */
    PAC &= (uint8_t)~(SDA_BIT | SCL_BIT);   /* обе линии отпущены */

    /* write-only: пишем целиком. PA0/PA5 — цифровые входы, PA4 отключён
       (аналоговый вход компаратора, буфер ноги давит утечками). */
    PADIER = SCL_BIT | SDA_BIT;

    /* Слабые подтяжки (как в оригинале) — резерв к внешним 4.7к. */
    PAPH = SCL_BIT | SDA_BIT;

    state = ST_IDLE;
    data = 0;
    len = 0;
    ptr = 0;
    i2c_rw = 0;
    pause_sda = 0;
    last = PA;
}

void main(void)
{
    i2c_init();

    while (1)
    {
        uint8_t current = PA;
        scl_change = (uint8_t)((last ^ current) & SCL_BIT);
        sda_change = (uint8_t)((last ^ current) & SDA_BIT);
        last = current;

        if (scl_change)
            check_scl();

        if (sda_change)
            check_sda();

        /* Измерение по переполнению тика, только пока шина свободна:
           во время него (~0.4 мс) слейв слепнет, мастер ретраит. */
        if (++tick == 0 && state == ST_IDLE)
        {
            int8_t nvdd  = get_nvdd();
            int8_t nvpa4 = get_nvpa4();
            vdd_v  = get_vdd(nvdd);              /* 0 = вне шкалы */
            pa4_v = get_vpa4(nvpa4, nvdd);
        }
    }
}

/*
  Startup code — настройка и калибровка системного клока.
  Выполняется до инициализации данных; return 0 — продолжить инициализацию.
*/
unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);
    return 0;
}
