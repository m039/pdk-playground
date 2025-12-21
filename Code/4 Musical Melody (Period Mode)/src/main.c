#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"
#include "melodies/komarovo_optimized.h"

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define BUTTON_BIT 4 // PA4

#define isButtonActive()    !(PA & (1 << BUTTON_BIT))

#define sleep() __asm stopsys __endasm;

void delay(uint16_t time) {
  for (uint16_t i = 0; i < time; i++) {
    _delay_ms(1);
  }
}

void playMelody() {
  for (int thisNote = 0; thisNote < MELODY_SIZE; thisNote++) {   
    tone(MELODY_TONE(thisNote));
    delay(MELODY_DURATION(thisNote));
    
    tone(0);
    delay(MELODY_NO_TONE_DURATION(thisNote));

    if (isButtonActive()) {
      break;
    }
  }
}

void main() {
  PAC |= (1 << BUZZER_BIT);
  PADIER |= (1 << BUTTON_BIT);
  PAPH |= (1 << BUTTON_BIT);

  TM2B = 0;
  TM2C = (uint8_t)(TM2C_MODE_PERIOD | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
  TM2S = 0x0;

  uint8_t clkmd = CLKMD;

  while (1) {
    if (isButtonActive()) {
      CLKMD = clkmd;
      playMelody();
    }
    
    CLKMD = 0xF4;
    CLKMD &= ~CLKMD_ENABLE_IHRC;

    sleep();
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
