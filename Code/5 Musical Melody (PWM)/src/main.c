#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

// Original notes
// #define NOTE_C4  262
// #define NOTE_D4  294
// #define NOTE_E4  330
// #define NOTE_F4  349
// #define NOTE_G4  392
// #define NOTE_A4  440
// #define NOTE_B4  494
// #define NOTE_C5  523
// #define REST     0

#define NOTE_C4  (2 << 5 | 15)  // 260HZ
#define NOTE_D4  (2 << 5 | 13)  // 300HZ
#define NOTE_E4  (2 << 5 | 12)  // 325Hz
#define NOTE_F4  (2 << 5 | 11)  // 355Hz
#define NOTE_G4  (2 << 5 | 10)  // 390Hz
#define NOTE_A4  (2 << 5 | 9)   // 434Hz
#define NOTE_B4  (2 << 5 | 8)   // 488Hz
#define NOTE_C5  (2 << 5 | 7)   // 558Hz
#define REST     0

// Simple melody: "Twinkle Twinkle Little Star"
const uint8_t melody[] = {
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

#define TONE_FREQ (16000000 / 64)
#define MAX_SCALE (32)

void tone(uint8_t frequency) {
  if (frequency == 0) {
    TM2C = 0;
  } else {
    TM2C = (uint8_t)(TM2C_MODE_PWM | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
    TM2S = frequency;
    TM2B = 0x7f;
  }
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
