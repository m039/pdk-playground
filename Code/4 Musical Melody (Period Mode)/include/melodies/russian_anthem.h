#ifndef RUSSIAN_ANTHEM
#define RUSSIAN_ANTHEM

#include <stdint.h>
#include "delay.h"

#define NOTE_G4 1
#define NOTE_D4 2
#define NOTE_E4 3
#define NOTE_Fb4 4
#define NOTE_B3 5
#define NOTE_C4 6
#define NOTE_G3 7
#define NOTE_A3 8
#define NOTE_A4 9
#define NOTE_B4 10
#define NOTE_C5 11

#define TONE_FREQ (16000000 / (2 * 64 * 3))
#define F(x) (uint8_t)(TONE_FREQ / x - 1)

const uint8_t melody_tones[] = {
    0,
    F(392), // G4
    F(294), // D4
    F(330), // E4
    F(370), // Fb4
    F(247), // B3
    F(262), // C4
    F(196), // G3
    F(220), // A3
    F(440), // A4
    F(494), // B4
    F(523), // C5
};

#define DURATION_0 0
#define DURATION_125 1
#define DURATION_250 2
#define DURATION_375 3
#define DURATION_500 4
#define DURATION_750 5
#define DURATION_1000 6
#define DURATION_1500 7
#define DURATION_2000 8

const uint16_t melody_durations[] = {
    0, // 0
    125, // 1
    250, // 2
    375, // 3
    500, // 4
    750, // 5
    1000, // 6
    1500, // 7
    2000, // 8
};

typedef uint16_t melody_data;

// Generated from https://arduinomidi.netlify.app/

const melody_data melody[102] = {
 (NOTE_G4 << 8) | DURATION_1500,
 (NOTE_D4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_D4 << 8) | DURATION_375,
 (NOTE_E4 << 8) | DURATION_125,
 (NOTE_Fb4 << 8) | DURATION_500,
 (NOTE_B3 << 8) | DURATION_375,
 (NOTE_B3 << 8) | DURATION_125,
 (NOTE_E4 << 8) | DURATION_500,
 (NOTE_D4 << 8) | DURATION_375,
 (NOTE_C4 << 8) | DURATION_125,
 (NOTE_D4 << 8) | DURATION_500,
 (NOTE_G3 << 8) | DURATION_375,
 (NOTE_G3 << 8) | DURATION_125,
 (NOTE_A3 << 8) | DURATION_500,
 (NOTE_A3 << 8) | DURATION_375,
 (NOTE_B3 << 8) | DURATION_125,
 (NOTE_C4 << 8) | DURATION_500,
 (NOTE_C4 << 8) | DURATION_375,
 (NOTE_D4 << 8) | DURATION_125,
 (NOTE_E4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_375,
 (NOTE_G4 << 8) | DURATION_125,
 (NOTE_A4 << 8) | DURATION_750,
 (NOTE_D4 << 8) | DURATION_250,
 (NOTE_B4 << 8) | DURATION_500,
 (NOTE_A4 << 8) | DURATION_375,
 (NOTE_G4 << 8) | DURATION_125,
 (NOTE_A4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_375,
 (NOTE_D4 << 8) | DURATION_125,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_375,
 (NOTE_E4 << 8) | DURATION_125,
 (NOTE_Fb4 << 8) | DURATION_500,
 (NOTE_B3 << 8) | DURATION_375,
 (NOTE_B3 << 8) | DURATION_125,
 (NOTE_E4 << 8) | DURATION_500,
 (NOTE_D4 << 8) | DURATION_375,
 (NOTE_C4 << 8) | DURATION_125,
 (NOTE_D4 << 8) | DURATION_500,
 (NOTE_G3 << 8) | DURATION_375,
 (NOTE_G3 << 8) | DURATION_125,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_375,
 (NOTE_E4 << 8) | DURATION_125,
 (NOTE_D4 << 8) | DURATION_250,
 (NOTE_Fb4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_A4 << 8) | DURATION_250,
 (NOTE_B4 << 8) | DURATION_1000,
 (NOTE_A4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_Fb4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_A4 << 8) | DURATION_750,
 (NOTE_D4 << 8) | DURATION_250,
 (NOTE_D4 << 8) | DURATION_1000,
 (NOTE_G4 << 8) | DURATION_1000,
 (NOTE_Fb4 << 8) | DURATION_250,
 (NOTE_E4 << 8) | DURATION_250,
 (NOTE_D4 << 8) | DURATION_250,
 (NOTE_E4 << 8) | DURATION_250,
 (NOTE_Fb4 << 8) | DURATION_750,
 (NOTE_B3 << 8) | DURATION_250,
 (NOTE_B3 << 8) | DURATION_1000,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_E4 << 8) | DURATION_375,
 (NOTE_Fb4 << 8) | DURATION_125,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_E4 << 8) | DURATION_375,
 (NOTE_Fb4 << 8) | DURATION_125,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_E4 << 8) | DURATION_375,
 (NOTE_G4 << 8) | DURATION_125,
 (NOTE_C5 << 8) | DURATION_1000,
 (NOTE_C5 << 8) | DURATION_1000,
 (NOTE_B4 << 8) | DURATION_250,
 (NOTE_A4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_A4 << 8) | DURATION_250,
 (NOTE_B4 << 8) | DURATION_750,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_1000,
 (NOTE_A4 << 8) | DURATION_1000,
 (NOTE_G4 << 8) | DURATION_250,
 (NOTE_Fb4 << 8) | DURATION_250,
 (NOTE_E4 << 8) | DURATION_250,
 (NOTE_Fb4 << 8) | DURATION_250,
 (NOTE_G4 << 8) | DURATION_750,
 (NOTE_E4 << 8) | DURATION_250,
 (NOTE_E4 << 8) | DURATION_1000,
 (NOTE_G4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_375,
 (NOTE_E4 << 8) | DURATION_125,
 (NOTE_D4 << 8) | DURATION_500,
 (NOTE_G3 << 8) | DURATION_375,
 (NOTE_G3 << 8) | DURATION_125,
 (NOTE_D4 << 8) | DURATION_1000,
 (NOTE_E4 << 8) | DURATION_500,
 (NOTE_Fb4 << 8) | DURATION_500,
 (NOTE_G4 << 8) | DURATION_2000
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) melody_tones[melody[x] >> 8]
#define MELODY_NO_TONE_DURATION(x) 25
#define MELODY_DURATION(x) melody_durations[melody[x] & 0xFF]

void tone(uint8_t frequency)
{
  if (frequency <= 0) {
    TM2B = 0;
  } else {
    TM2S = 3 << 5 | 2;
    TM2B = frequency;
  }
}

#endif