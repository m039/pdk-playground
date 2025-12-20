#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"
#include "melodies/imperial_march.h"

#define PWM_MAX               255

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define TONE_COMPLEX 0

void tone(long frequency) {
#if TONE_COMPLEX
#define TONE_FREQ (16000000)
#define MAX_SCALE (32 * 256)

  if (frequency <= 0) {
    TM2B = 0;
  } else {
    long scale = TONE_FREQ / 2 / frequency;
    uint8_t s1 = 0;

    if (scale > MAX_SCALE) {
      scale = TONE_FREQ / 2 / frequency / 4;
      s1 = 1;
      if (scale > MAX_SCALE) {
        scale = TONE_FREQ / 2 / frequency / 16;
        s1 = 2;
        if (scale > MAX_SCALE) {
          scale = TONE_FREQ / 2 / frequency / 64;
          s1 = 3;
        }
      }
    } else {
      TM2B = 0;
      return;
    }

    if (scale >= 256) {
      scale = scale / 256;
      TM2B = 0xFF;
      TM2S = s1 << 5 | (scale) & 0x1F;
    } else {
      TM2B = scale;
      TM2S = s1 << 5 | 0;
    }
  }
#else 
  #define TONE_FREQ (16000000 / (2 * 64 * 2))

  if (frequency <= 0) {
    TM2B = 0;
  } else {
    TM2S = 3 << 5 | 1;
    TM2B = TONE_FREQ / frequency  - 1;
  }
#endif
}

void playMelody() {
  for (int thisNote = 0; thisNote < MELODY_SIZE; thisNote++) {   
    tone(MELODY_TONE(thisNote));
    _delay_loop_32(MELODY_DURATION(thisNote));
    
    tone(0);
    _delay_ms(40);
  }
}

void main() {
  PAC |= (1 << BUZZER_BIT);
 
  TM2B = 0;
  TM2C = (uint8_t)(TM2C_MODE_PERIOD | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
  TM2S = 0x0;

  while (1) {
     playMelody();

    _delay_ms(4000);
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
