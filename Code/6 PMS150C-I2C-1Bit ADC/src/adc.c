#include "adc.h"

static uint8_t i;

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
  Реализовано через switch для экономии ROM и исключения __gptrget библиотеки SDCC.
*/
uint8_t get_vdd(int8_t nvdd)
{
    uint8_t idx = clamp_idx(nvdd);
    switch (idx) {
        case 0: return 255;
        case 1: return 192;
        case 2: return 128;
        case 3: return 96;
        case 4: return 76;
        case 5: return 64;
        case 6: return 54;
        case 7: return 48;
        case 8: return 42;
        case 9: return 38;
        case 10: return 34;
        case 11: return 32;
        case 12: return 29;
        case 13: return 27;
        case 14: return 25;
        default: return 24;
    }
}

/*
  Пересчёт PA4 в децимилливольты: PA4 = (nvpa4+1)/32 * VDD.
*/
uint8_t get_vpa4(int8_t nvpa4, uint8_t vdd)
{
    uint8_t count = clamp_idx(nvpa4) + 1;
    uint16_t res = 0;
    do {
        res += vdd;
    } while (--count);

    return (uint8_t)(res >> 5);
}
