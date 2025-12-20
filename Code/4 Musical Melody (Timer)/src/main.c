#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define REST     0

// Simple melody: "Twinkle Twinkle Little Star"
const int melody[] = {
  NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, REST,
  NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, REST
};

#define D4 LOOP_CTR_32(MS_TO_CYCLES(1000 / 4))
#define D2 LOOP_CTR_32(MS_TO_CYCLES(1000 / 2))

#define DP4 (D4 * 1.3)
#define DP2 (D2 * 1.3)

// Note durations: 4 = quarter note, 8 = eighth note, etc.
const uint32_t noteDurations[] = {
  D4, D4, D4, D4, D4, D4, D2, D4,
  D4, D4, D4, D4, D4, D4, D2, D4
};

const uint32_t notePause[] = {
  DP4, DP4, DP4, DP4, DP4, DP4, DP2, DP4,
  DP4, DP4, DP4, DP4, DP4, DP4, DP2, DP4
};

#define PWM_MAX               255

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define TONE_COMPLEX 1

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
  #define TONE_FREQ (16000000 / (2 * 64 * 16))

  if (frequency <= 0) {
    TM2B = 0;
  } else {
    TM2S = 3 << 5 | 15;
    TM2B = TONE_FREQ / frequency  - 1;
  }
#endif
}

void playMelody() {
  for (int thisNote = 0; thisNote < 16; thisNote++) {   
    tone(melody[thisNote]);
    _delay_loop_32(noteDurations[thisNote]);
    
    tone(0);
    _delay_loop_32(notePause[thisNote]);
  }
}

void main() {
  PAC |= (1 << BUZZER_BIT);
 
  TM2B = 0;
  TM2C = (uint8_t)(TM2C_INVERT_OUT | TM2C_MODE_PERIOD | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
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
