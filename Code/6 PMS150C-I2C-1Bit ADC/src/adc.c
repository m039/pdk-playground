#include "adc.h"

static uint8_t i;

/* Case 4: ступень N лестницы = (N+1)/32 * VDD; VDD = 120*32/(N+1) децимилливольт. */
#define VOLTAGE(n) (uint8_t)((12. * 32. / (n + 1.)) > 255 ? 0xff: (12. * 32. / (n + 1.)))

static const uint8_t voltages[16] = {
    VOLTAGE(0),  VOLTAGE(1),  VOLTAGE(2),  VOLTAGE(3),
    VOLTAGE(4),  VOLTAGE(5),  VOLTAGE(6),  VOLTAGE(7),
    VOLTAGE(8),  VOLTAGE(9),  VOLTAGE(10), VOLTAGE(11),
    VOLTAGE(12), VOLTAGE(13), VOLTAGE(14), VOLTAGE(15)
};

/*
  Измерение VDD: сколько ступеней лестницы (Case 4) ещё выше bandgap 1.2 В.
  PLUS  = VINT_R = (N+1)/32 * VDD  — порог РАСТЁТ с ростом N;
  MINUS = bandgap 1.2 В.
*/
int8_t get_nvdd(void)
{
    // Внутренние входы: ножки не участвуют, утечки не влияют.
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);
    for (i = 0; i < 16; i++)
    {
        // GPCS пишем ЦЕЛИКОМ: биты case [5:4] и уровня [3:0] write-only
        GPCS = (1 << 5) | (1 << 4) | i; // Case 4
        _delay_us(10);                  // установление компаратора после смены порога
        if (GPCC & GPCC_COMP_RESULT_POSITIVE)
        {
            return (i == 0) ? ADC_ABOVE_MAX : (int8_t)i;
        }
    }
    return ADC_BELOW_MIN; // лестница кончилась: VDD <= ADC_VDD_MIN
}

/*
  Измерение напряжения на PA4 той же лестницей (Case 4).
  PLUS  = PA4;
  MINUS = VINT_R = (N+1)/32 * VDD.
*/
int8_t get_nvpa4(void)
{
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_VINT_R | GPCC_COMP_PLUS_PA4);
    for (i = 0; i < 16; i++)
    {
        GPCS = (1 << 5) | (1 << 4) | i; // Case 4
        _delay_us(10);
        if (!(GPCC & GPCC_COMP_RESULT_POSITIVE))
        {
            return (i == 0) ? ADC_BELOW_MIN : (int8_t)i;
        }
    }
    return ADC_ABOVE_MAX;
}

static uint8_t clamp_idx(int8_t val)
{
    if (val == ADC_ABOVE_MAX) return 15;
    if (val < 0) return 0;
    return (uint8_t)val;
}

/*
  Пересчёт индекса ступени в VDD (децимилливольты, 1 ед. = 100 мВ).
*/
uint8_t get_vdd(int8_t nvdd)
{
    return voltages[clamp_idx(nvdd)];
}

/*
  Пересчёт PA4 в децимилливольты: PA4 = (nvpa4+1)/32 * VDD.
*/
uint8_t get_vpa4(int8_t nvpa4, int8_t nvdd)
{
    uint8_t vdd = voltages[clamp_idx(nvdd)];
    uint8_t count = clamp_idx(nvpa4) + 1;
    uint16_t res = 0;
    do {
        res += vdd;
    } while (--count);

    return (uint8_t)(res >> 5);
}
