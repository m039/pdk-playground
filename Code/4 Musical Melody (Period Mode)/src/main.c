#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"
#include "melodies/russian_anthem.h"


#define PWM_MAX               255

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define BUTTON_BIT 4 // PA4

void playMelody() {
  for (int thisNote = 0; thisNote < MELODY_SIZE; thisNote++) {   
    tone(MELODY_TONE(thisNote));
    _delay_loop_32(MELODY_DURATION(thisNote));
    
    tone(0);
    _delay_loop_32(MELODY_NO_TONE(thisNote));
  }
}

void main() {
  // PAC &= ~(1 << BUTTON_BIT);
  PAC |= (1 << BUZZER_BIT);
  PADIER |= (1 << BUTTON_BIT);
  PAPH |= (1 << BUTTON_BIT);

  TM2B = 0;
  TM2C = (uint8_t)(TM2C_MODE_PERIOD | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
  TM2S = 0x0;

  while (1) {
    if (!(PA & (1 << BUTTON_BIT))) {
      playMelody();
    }
    _delay_ms(1);
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
