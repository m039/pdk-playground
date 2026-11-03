#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

/*
  Voltage Supervisor Threshold Configuration:
  Configure ADC_VOLTAGE_THRESHOLD in platformio.ini (recommended):
    build_flags = -DADC_VOLTAGE_THRESHOLD=ADC_VOLTAGE_3V20
  or uncomment and set it directly below.

  Note: Threshold voltages may vary with a 1-2% tolerance depending
  on the operating frequency of the microcontroller.

  Available threshold constants from adc.h (Comparator Case 4):
    ADC_VOLTAGE_2V40 (24) - 2.40 V (N = 15)
    ADC_VOLTAGE_2V56 (25) - 2.56 V (N = 14)
    ADC_VOLTAGE_2V74 (27) - 2.74 V (N = 13)
    ADC_VOLTAGE_2V95 (29) - 2.95 V (N = 12)
    ADC_VOLTAGE_3V20 (32) - 3.20 V (N = 11)
    ADC_VOLTAGE_3V49 (34) - 3.49 V (N = 10)
    ADC_VOLTAGE_3V84 (38) - 3.84 V (N =  9)
    ADC_VOLTAGE_4V27 (42) - 4.27 V (N =  8)
    ADC_VOLTAGE_4V80 (48) - 4.80 V (N =  7)
    ADC_VOLTAGE_5V49 (54) - 5.49 V (N =  6)
*/
// #define ADC_VOLTAGE_THRESHOLD ADC_VOLTAGE_3V20

#ifndef ADC_VOLTAGE_THRESHOLD
#error "ADC_VOLTAGE_THRESHOLD is not defined! Please select a threshold in platformio.ini (e.g. build_flags = -DADC_VOLTAGE_THRESHOLD=ADC_VOLTAGE_3V20) or define it in main.c."
#elif !((ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_2V40) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_2V56) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_2V74) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_2V95) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_3V20) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_3V49) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_3V84) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_4V27) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_4V80) || \
        (ADC_VOLTAGE_THRESHOLD == ADC_VOLTAGE_5V49))
#error "Invalid ADC_VOLTAGE_THRESHOLD value! Must be one of the Case 4 comparator thresholds from adc.h: ADC_VOLTAGE_2V40 (24), ADC_VOLTAGE_2V56 (25), ADC_VOLTAGE_2V74 (27), ADC_VOLTAGE_2V95 (29), ADC_VOLTAGE_3V20 (32), ADC_VOLTAGE_3V49 (34), ADC_VOLTAGE_3V84 (38), ADC_VOLTAGE_4V27 (42), ADC_VOLTAGE_4V80 (48), ADC_VOLTAGE_5V49 (54)."
#endif

#define CMP_OUT_BIT 4

void main(void)
{
    uint8_t wait = 0;
    CLKMD = 0xF4;
    CLKMD &= ~CLKMD_ENABLE_IHRC;
    CLKMD = 0x4C;

    PAC |= (1 << CMP_OUT_BIT);

    uint8_t nvdd = 0;
    for (int8_t i = 15; i >= 0; i--) {
        if (ADC_VOLTAGE_THRESHOLD <= get_vdd(i)) {
            nvdd = i + 1; 
            break;
        }
    }

    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);
    GPCS =  (1 << 5) | (1 << 4) | nvdd;
    while (++wait != 0) {}

    while (1)
    {
        uint8_t cmp = (GPCC & GPCC_COMP_RESULT_POSITIVE) == GPCC_COMP_RESULT_POSITIVE;
        uint8_t pa = (PA & (1 << CMP_OUT_BIT)) == (1 << CMP_OUT_BIT);
        
        if (cmp && !pa)
        {
            PA |= 1 << CMP_OUT_BIT;
            GPCS = (1 << 5) | (1 << 4) | (nvdd);
            while (++wait != 0) {}
        }

        if (!cmp && pa) {
            PA &= ~(1 << CMP_OUT_BIT);
            GPCS = (1 << 5) | (1 << 4) | (nvdd - 1);
            while (++wait != 0) {}
        }              
    }
}

/*
  Startup code - setup and calibrate internal system clock.
*/
unsigned char _sdcc_external_startup(void)
{
    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

    return 0;
}
