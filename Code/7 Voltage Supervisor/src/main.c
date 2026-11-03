#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define ADC_VOLTAGE_THRESHOLD 32 // 3.2V

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
