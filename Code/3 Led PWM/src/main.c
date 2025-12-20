#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

#define PWM_MAX               255

#define LED_BIT 3 // PA3 (TM2PWM)

void main() {
  PAC |= (1 << LED_BIT);
 
  TM2B = 0x00;
  TM2C = (uint8_t)(TM2C_INVERT_OUT | TM2C_MODE_PWM | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
  TM2S = 0x0;

  while (1) {
    uint8_t fadeValue;

    for (fadeValue = 0; fadeValue < PWM_MAX; fadeValue += 5) {
      TM2B = fadeValue;
      _delay_ms(30);
    }

    for (fadeValue = PWM_MAX; fadeValue > 0; fadeValue -= 5) {
      TM2B = fadeValue;
      _delay_ms(30);
    }
  }
}

// Startup code - Setup/calibrate system clock
unsigned char _sdcc_external_startup(void) {

  // Initialize the system clock (CLKMD register) with the IHRC, ILRC, or EOSC clock source and correct divider.
  // The AUTO_INIT_SYSCLOCK() macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC clock source and divider.
  // Alternatively, replace this with the more specific PDK_SET_SYSCLOCK(...) macro from pdk/sysclock.h
  AUTO_INIT_SYSCLOCK();

  // Insert placeholder code to tell EasyPdkProg to calibrate the IHRC or ILRC internal oscillator.
  // The AUTO_CALIBRATE_SYSCLOCK(...) macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC oscillator.
  // Alternatively, replace this with the more specific EASY_PDK_CALIBRATE_IHRC(...) or EASY_PDK_CALIBRATE_ILRC(...) macro from easy-pdk/calibrate.h
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  return 0;   // Return 0 to inform SDCC to continue with normal initialization.
}
