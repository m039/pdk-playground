#ifndef _MOSCOW_IN_MAY
#define _MOSCOW_IN_MAY

// https://www.youtube.com/watch?v=t892M-wR3uQ&list=RDt892M-wR3uQ&start_radio=1
// https://musescore.com/the_alex/scores/7054048

#include <stdint.h>
#include "delay.h"

#define TONE_FREQ (16000000 / (2 * 64 * 3))
#define F(x) (uint8_t)(TONE_FREQ / x - 1)

#define NOTE_A4 0
#define NOTE_Ab4 1
#define NOTE_C5 2
#define NOTE_G4 3
#define NOTE_F4 4
#define NOTE_D5 5
#define NOTE_E4 6
#define NOTE_D4 7
#define NOTE_E5 8
#define NOTE_F5 9
#define NOTE_G5 10
#define NOTE_Cb5 11

const uint8_t melody_tones[] = {
  0,
  F(440), // A4
  F(466), // Ab4
  F(523), // C5
  F(392), // G4
  F(349), // F4
  F(587), // D5
  F(330), // E4
  F(294), // D4
  F(659), // E5
  F(698), // F5
  F(784), // G5
  F(554), // Cb5
};
#define DURATION_0 0
#define DURATION_275 1
#define DURATION_150 2
#define DURATION_25 3
#define DURATION_425 4
#define DURATION_300 5
#define DURATION_175 6
#define DURATION_50 7
#define DURATION_1075 8
#define DURATION_450 9
#define DURATION_325 10
#define DURATION_200 11
#define DURATION_75 12
#define DURATION_1100 13
#define DURATION_850 14
#define DURATION_475 15
#define DURATION_350 16
#define DURATION_225 17
#define DURATION_100 18
#define DURATION_1125 19
#define DURATION_375 20
#define DURATION_250 21
#define DURATION_125 22

const uint32_t melody_durations[] = {
  LOOP_CTR_32(MS_TO_CYCLES(0)), // 0
  LOOP_CTR_32(MS_TO_CYCLES(275)), // 1
  LOOP_CTR_32(MS_TO_CYCLES(150)), // 2
  LOOP_CTR_32(MS_TO_CYCLES(25)), // 3
  LOOP_CTR_32(MS_TO_CYCLES(425)), // 4
  LOOP_CTR_32(MS_TO_CYCLES(300)), // 5
  LOOP_CTR_32(MS_TO_CYCLES(175)), // 6
  LOOP_CTR_32(MS_TO_CYCLES(50)), // 7
  LOOP_CTR_32(MS_TO_CYCLES(1075)), // 8
  LOOP_CTR_32(MS_TO_CYCLES(450)), // 9
  LOOP_CTR_32(MS_TO_CYCLES(325)), // 10
  LOOP_CTR_32(MS_TO_CYCLES(200)), // 11
  LOOP_CTR_32(MS_TO_CYCLES(75)), // 12
  LOOP_CTR_32(MS_TO_CYCLES(1100)), // 13
  LOOP_CTR_32(MS_TO_CYCLES(850)), // 14
  LOOP_CTR_32(MS_TO_CYCLES(475)), // 15
  LOOP_CTR_32(MS_TO_CYCLES(350)), // 16
  LOOP_CTR_32(MS_TO_CYCLES(225)), // 17
  LOOP_CTR_32(MS_TO_CYCLES(100)), // 18
  LOOP_CTR_32(MS_TO_CYCLES(1125)), // 19
  LOOP_CTR_32(MS_TO_CYCLES(375)), // 20
  LOOP_CTR_32(MS_TO_CYCLES(250)), // 21
  LOOP_CTR_32(MS_TO_CYCLES(125)), // 22
};

typedef uint16_t melody_data;

const melody_data melody[92] = {
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_50},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_A4 << 12) | (DURATION_150 << 6) | DURATION_75},
  {(NOTE_Ab4 << 12) | (DURATION_75 << 6) | DURATION_200},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_C5 << 12) | (DURATION_50 << 6) | DURATION_325},
  {(NOTE_Ab4 << 12) | (DURATION_375 << 6) | DURATION_125},
  {(NOTE_Ab4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_G4 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_G4 << 12) | (DURATION_150 << 6) | DURATION_75},
  {(NOTE_F4 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_G4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_Ab4 << 12) | (DURATION_50 << 6) | DURATION_325},
  {(NOTE_A4 << 12) | (DURATION_325 << 6) | DURATION_150},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_100 << 6) | DURATION_300},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_Ab4 << 12) | (DURATION_75 << 6) | DURATION_175},
  {(NOTE_A4 << 12) | (DURATION_100 << 6) | DURATION_125},
  {(NOTE_C5 << 12) | (DURATION_25 << 6) | DURATION_225},
  {(NOTE_Ab4 << 12) | (DURATION_375 << 6) | DURATION_125},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_50},
  {(NOTE_Ab4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_275},
  {(NOTE_G4 << 12) | (DURATION_75 << 6) | DURATION_50},
  {(NOTE_G4 << 12) | (DURATION_50 << 6) | DURATION_175},
  {(NOTE_D5 << 12) | (DURATION_350 << 6) | DURATION_425},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_A4 << 12) | (DURATION_75 << 6) | DURATION_150},
  {(NOTE_Ab4 << 12) | (DURATION_75 << 6) | DURATION_200},
  {(NOTE_A4 << 12) | (DURATION_25 << 6) | DURATION_75},
  {(NOTE_C5 << 12) | (DURATION_25 << 6) | DURATION_375},
  {(NOTE_Ab4 << 12) | (DURATION_350 << 6) | DURATION_125},
  {(NOTE_Ab4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_G4 << 12) | (DURATION_100 << 6) | DURATION_275},
  {(NOTE_G4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_F4 << 12) | (DURATION_50 << 6) | DURATION_225},
  {(NOTE_G4 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_Ab4 << 12) | (DURATION_50 << 6) | DURATION_325},
  {(NOTE_A4 << 12) | (DURATION_325 << 6) | DURATION_175},
  {(NOTE_F4 << 12) | (DURATION_75 << 6) | DURATION_50},
  {(NOTE_F4 << 12) | (DURATION_75 << 6) | DURATION_50},
  {(NOTE_F4 << 12) | (DURATION_25 << 6) | DURATION_225},
  {(NOTE_F4 << 12) | (DURATION_125 << 6) | DURATION_125},
  {(NOTE_E4 << 12) | (DURATION_25 << 6) | DURATION_200},
  {(NOTE_F4 << 12) | (DURATION_75 << 6) | DURATION_75},
  {(NOTE_A4 << 12) | (DURATION_25 << 6) | DURATION_375},
  {(NOTE_G4 << 12) | (DURATION_275 << 6) | DURATION_200},
  {(NOTE_G4 << 12) | (DURATION_75 << 6) | DURATION_50},
  {(NOTE_G4 << 12) | (DURATION_100 << 6) | DURATION_50},
  {(NOTE_G4 << 12) | (DURATION_175 << 6) | DURATION_75},
  {(NOTE_F4 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_E4 << 12) | (DURATION_25 << 6) | DURATION_100},
  {(NOTE_D4 << 12) | (DURATION_450 << 6) | DURATION_150},
  {(NOTE_D5 << 12) | (DURATION_50 << 6) | DURATION_200},
  {(NOTE_E5 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_D5 << 12) | (DURATION_50 << 6) | DURATION_325},
  {(NOTE_C5 << 12) | (DURATION_1100 << 6) | DURATION_125},
  {(NOTE_E5 << 12) | (DURATION_175 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_50 << 6) | DURATION_350},
  {(NOTE_F5 << 12) | (DURATION_1125 << 6) | DURATION_125},
  {(NOTE_F5 << 12) | (DURATION_125 << 6) | DURATION_125},
  {(NOTE_G5 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_F5 << 12) | (DURATION_25 << 6) | DURATION_350},
  {(NOTE_E5 << 12) | (DURATION_1100 << 6) | DURATION_150},
  {(NOTE_E5 << 12) | (DURATION_100 << 6) | DURATION_125},
  {(NOTE_F5 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_50 << 6) | DURATION_350},
  {(NOTE_D5 << 12) | (DURATION_1125 << 6) | DURATION_125},
  {(NOTE_D5 << 12) | (DURATION_175 << 6) | DURATION_100},
  {(NOTE_E5 << 12) | (DURATION_50 << 6) | DURATION_75},
  {(NOTE_D5 << 12) | (DURATION_25 << 6) | DURATION_325},
  {(NOTE_C5 << 12) | (DURATION_850 << 6) | DURATION_175},
  {(NOTE_E5 << 12) | (DURATION_175 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_175 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_175 << 6) | DURATION_75},
  {(NOTE_E5 << 12) | (DURATION_25 << 6) | DURATION_225},
  {(NOTE_F5 << 12) | (DURATION_1125 << 6) | DURATION_125},
  {(NOTE_F5 << 12) | (DURATION_200 << 6) | DURATION_75},
  {(NOTE_F5 << 12) | (DURATION_150 << 6) | DURATION_75},
  {(NOTE_F5 << 12) | (DURATION_25 << 6) | DURATION_250},
  {(NOTE_E5 << 12) | (DURATION_1075 << 6) | DURATION_150},
  {(NOTE_A4 << 12) | (DURATION_25 << 6) | DURATION_225},
  {(NOTE_Cb5 << 12) | (DURATION_75 << 6) | DURATION_175},
  {(NOTE_D5 << 12) | (DURATION_0 << 6) | DURATION_475},
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) ((melody[x] >> 12) & 0xf)
#define MELODY_NO_TONE_DURATION(x) melody_durations[(melody[x] >> 6) & 0x3f]
#define MELODY_DURATION(x) melody_durations[melody[x] & 0x3F]

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