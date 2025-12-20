#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

#define Nc    261
#define Nd    294
#define Ne    329
#define Nf    349
#define Ng    391
#define NgS   415 // G Sharp
#define Na    440
#define NaS   455 // A Sharp
#define Nb    466
#define NcH   523 // C High
#define NcSH  554 // C Sharp High
#define NdH   587 // D High
#define NdSH  622 // D Sharp High
#define NeH   659 // E High
#define NfH   698 // F High
#define NfSH  740 // F Sharp High
#define NgH   784 // G High
#define NgSH  830 // G Sharp High
#define NaH   880 // A High

#define REST 0

#define D650 LOOP_CTR_32(MS_TO_CYCLES(650))
#define D500 LOOP_CTR_32(MS_TO_CYCLES(500))
#define D375 LOOP_CTR_32(MS_TO_CYCLES(375))
#define D350 LOOP_CTR_32(MS_TO_CYCLES(350))
#define D325 LOOP_CTR_32(MS_TO_CYCLES(325))
#define D300 LOOP_CTR_32(MS_TO_CYCLES(300))
#define D250 LOOP_CTR_32(MS_TO_CYCLES(250))
#define D175 LOOP_CTR_32(MS_TO_CYCLES(175))
#define D150 LOOP_CTR_32(MS_TO_CYCLES(150))
#define D125 LOOP_CTR_32(MS_TO_CYCLES(125))

// Melody taken from https://medium.com/@bigmaitz/using-arduino-to-play-star-wars-imperial-march-tune-47f75a6e1e43

const int melody[] = {
  Na, Na, Na, Nf, NcH, Na, Nf, NcH, Na, REST, // first
  NeH, NeH, NeH, NfH, NcH, NgS, Nf, NcH, Na, REST,

  NaH, Na, Na, NaH, NgSH, NgH, NfSH, NfH, NfSH, REST, // second
  NaS, NdSH, NdH, NcSH, NcH, Nb, NcH, REST,

  Nf, NgS, Nf, Na, NcH, Na, NcH, NeH, REST, // variant 1

  NaH, Na, Na, NaH, NgSH, NgH, NfSH, NfH, NfSH, REST, // second
  NaS, NdSH, NdH, NcSH, NcH, Nb, NcH, REST,

  Nf, NgS, Nf, NcH, Na, Nf, NcH, Na, REST // variant 2
};

const uint32_t noteDurations[] = {
  D500, D500, D500, D350, D150, D500, D350, D150, D650, D500, 
  D500, D500, D500, D350, D150, D500, D350, D150, D650, D500,

  D500, D300, D150, D500, D325, D175, D125, D125, D250, D500,
  D250, D500, D325, D175, D125, D125, D250, D500,

  D250, D500, D350, D125, D500, D375, D125, D650, D500,

  D500, D300, D150, D500, D325, D175, D125, D125, D250, D500,
  D250, D500, D325, D175, D125, D125, D250, D500,

  D250, D500, D350, D125, D500, D375, D125, D650, D500
};

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
  for (int thisNote = 0; thisNote < 74; thisNote++) {   
    tone(melody[thisNote]);
    _delay_loop_32(noteDurations[thisNote]);
    
    tone(0);
    _delay_ms(40);
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
