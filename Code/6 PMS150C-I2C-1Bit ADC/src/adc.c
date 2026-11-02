#include "adc.h"

static uint8_t i;

/*
  Comparator Case 4 (GPCS[5:4] = 0b11, GPCS[3:0] = N in 0..15):
  Internal resistor ladder voltage: VINT_R = (N + 1) / 32 * VDD (range: 1/32 to 16/32 of VDD).

  1. VDD Measurement (compared against 1.20V internal bandgap reference):
     Switching condition: VINT_R = 1.20V  =>  VDD = 1.20V * 32 / (N + 1) = 38.4V / (N + 1)
     Measurable VDD range within MCU operating limits (2.40V .. 5.50V):
       N = 15: 2.40 V (minimum detectable VDD; < 2.40V returns ADC_BELOW_MIN)
       N = 14: 2.56 V
       N = 13: 2.74 V
       N = 12: 2.95 V
       N = 11: 3.20 V
       N = 10: 3.49 V
       N =  9: 3.84 V
       N =  8: 4.27 V
       N =  7: 4.80 V
       N =  6: 5.49 V (near 5.50V max operating rating)
       N <= 5: > 6.40 V (exceeds PMS150C absolute maximum VDD)

  2. PA4 Analog Measurement (compared against VINT_R):
     Measurable PA4 range: (1/32)*VDD to (16/32)*VDD = 0.031*VDD to 0.50*VDD
     - At VDD = 5.0V: 0.16 V to 2.50 V (step ~156 mV)
     - At VDD = 3.3V: 0.10 V to 1.65 V (step ~103 mV)
     Inputs > VDD/2 return ADC_ABOVE_MAX; inputs < VDD/32 return ADC_BELOW_MIN.
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
