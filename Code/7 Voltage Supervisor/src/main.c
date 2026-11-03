#include <pdk/device.h>
#include "auto_sysclock.h"
#include "adc.h"

#define sleep() __asm stopsys __endasm;

#define DEBUG 0

#if DEBUG != 0
#include "serial.h"
#include <stdlib.h>
#endif

void interrupt(void) __interrupt(0)
{
    #if DEBUG != 0
  if (INTRQ & INTRQ_TM2)
  {
    INTRQ &= ~INTRQ_TM2;        // сбросить флаг, иначе ISR зациклится
    serial_irq_handler();       // отправить следующий бит TX
  }
  #endif

  if (INTRQ & INTRQ_COMP) {
    INTRQ &= ~INTRQ_COMP;
  }
}

#if DEBUG != 0

void print_number(const char *prefix, uint16_t number)
{
  const uint8_t *p = prefix;
  while (*p != 0)
  {
    putchar(*p);
    p++;
  }
  char buffer[6];
  __itoa(number, buffer, 10);
  puts(buffer);
}
#endif

#define ADC_INTERRUPT_SOURCE_VDD 1
#define ADC_INTERRUPT_SOURCE_PA4 2

#define ADC_VOLTAGE_THRESHOLD 32 // 3.2V

#define ADC_INTERRUPT_SOURCE ADC_INTERRUPT_SOURCE_VDD

#define CMP_OUT_BIT 4

void main(void)
{
    uint8_t wait = 0;
    CLKMD = 0xF4;
    CLKMD &= ~CLKMD_ENABLE_IHRC;
    CLKMD = 0x4C;

    PAC |= (1 << CMP_OUT_BIT);

    #if DEBUG != 0
    serial_setup();
    INTRQ = 0;
    __engint();
    #endif

    #if ADC_INTERRUPT_SOURCE == ADC_INTERRUPT_SOURCE_VDD
    uint8_t nvdd = 0;
    for (int8_t i = 15; i >= 0; i--) {
        if (ADC_VOLTAGE_THRESHOLD <= get_vdd(i)) {
            nvdd = i + 1; 
            break;
        }
    }

    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);
    GPCS =  (1 << 6) | (1 << 5) | (1 << 4) | nvdd;
    while (++wait != 0) {}

    INTRQ = 0;
    __engint();
    #endif

    while (1)
    {
        #if DEBUG != 0
        print_number("NVDD: ", nvdd);
        #endif

        uint8_t cmp = (GPCC & GPCC_COMP_RESULT_POSITIVE) == GPCC_COMP_RESULT_POSITIVE;
        uint8_t pa = (PA & (1 << CMP_OUT_BIT)) == (1 << CMP_OUT_BIT);
        
        if (cmp && !pa)
        {
            PA |= 1 << CMP_OUT_BIT;
            GPCS = (1 << 6) | (1 << 5) | (1 << 4) | (nvdd);
            while (++wait != 0) {}
        }

        if (!cmp && pa) {
            PA &= ~(1 << CMP_OUT_BIT);
            GPCS =  (1 << 6) | (1 << 5) | (1 << 4) | (nvdd - 1);
            while (++wait != 0) {}
        }              
    }
}

/*
  Startup code - setup and calibrate internal system clock.
*/
unsigned char _sdcc_external_startup(void)
{
    // PDK_SET_FUSE(FUSE_IO_DRV_NORMAL | FUSE_BOOTUP_FAST);

    AUTO_INIT_SYSCLOCK();
    AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

    return 0;
}
