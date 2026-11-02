#include "adc.h"

static uint8_t i;

/*
  Comparator Case 4:
  Ladder step N voltage: VINT_R = (N + 1) / 32 * VDD.
  When VINT_R equals the 1.2V bandgap reference: VDD = 1.2V * 32 / (N + 1).
  In decivolts (units of 100 mV = 0.1 V): VDD = 12 * 32 / (N + 1).
*/
#define _VOLTAGE(n) (12. * 32. / (n + 1.))
#define VOLTAGE(n) ((uint8_t)(_VOLTAGE(n) > 255 ? 0xFF : _VOLTAGE(n)))

static const uint8_t voltages[16] = {
    VOLTAGE(0),  VOLTAGE(1),  VOLTAGE(2),  VOLTAGE(3),
    VOLTAGE(4),  VOLTAGE(5),  VOLTAGE(6),  VOLTAGE(7),
    VOLTAGE(8),  VOLTAGE(9),  VOLTAGE(10), VOLTAGE(11),
    VOLTAGE(12), VOLTAGE(13), VOLTAGE(14), VOLTAGE(15)
};

/*
  Measures VDD: compares internal 1.2V bandgap against the internal resistor ladder (Case 4).
  PLUS  = VINT_R = (N + 1) / 32 * VDD (threshold increases with N)
  MINUS = 1.2V bandgap reference
*/
int8_t get_nvdd(void)
{
    /* Use internal inputs only: no external pins involved, eliminating pin leakage */
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);

    for (i = 0; i < 16; i++)
    {
        /* GPCS is write-only: write full byte (Case 4 bits [5:4] + level [3:0]) */
        GPCS = (1 << 5) | (1 << 4) | i; /* Case 4 */
        _delay_us(10);                   /* Settle comparator after changing threshold */

        if (GPCC & GPCC_COMP_RESULT_POSITIVE)
        {
            return (i == 0) ? ADC_ABOVE_MAX : (int8_t)i;
        }
    }

    return ADC_BELOW_MIN; /* Reached end of ladder: VDD <= minimum detectable threshold */
}

/*
  Measures voltage on PA4 using the same internal resistor ladder (Case 4).
  PLUS  = PA4 (analog comparator input)
  MINUS = VINT_R = (N + 1) / 32 * VDD
*/
int8_t get_nvpa4(void)
{
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_VINT_R | GPCC_COMP_PLUS_PA4);

    for (i = 0; i < 16; i++)
    {
        GPCS = (1 << 5) | (1 << 4) | i; /* Case 4 */
        _delay_us(10);                   /* Settle comparator after changing threshold */

        if (!(GPCC & GPCC_COMP_RESULT_POSITIVE))
        {
            return (i == 0) ? ADC_BELOW_MIN : (int8_t)i;
        }
    }

    return ADC_ABOVE_MAX;
}

static uint8_t clamp_idx(int8_t val)
{
    if (val == ADC_ABOVE_MAX)
    {
        return 15;
    }
    if (val < 0)
    {
        return 0;
    }
    return (uint8_t)val;
}

/*
  Converts ladder step index to VDD in decivolts (1 unit = 100 mV).
*/
uint8_t get_vdd(int8_t nvdd)
{
    return voltages[clamp_idx(nvdd)];
}

/*
  Calculates PA4 voltage in decivolts (1 unit = 100 mV):
  VPA4 = (nvpa4 + 1) / 32 * VDD.
*/
uint8_t get_vpa4(int8_t nvpa4, int8_t nvdd)
{
    uint8_t vdd = voltages[clamp_idx(nvdd)];
    uint8_t count = clamp_idx(nvpa4) + 1;
    uint16_t res = 0;

    do
    {
        res += vdd;
    } while (--count);

    return (uint8_t)(res >> 5);
}
