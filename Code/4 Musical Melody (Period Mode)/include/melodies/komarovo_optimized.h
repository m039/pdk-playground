#ifndef _KOMAROVO_O_
#define _KOMAROVO_O_

// https://www.youtube.com/watch?v=G6NqhIYzpZM&list=RDACYB1XNs04w&index=4
// https://www.midi-karaoke.info/215c2f1d.ru.html

#include <stdint.h>
#include "delay.h"

#define TONE_FREQ (16000000 / (2 * 64 * 3))
#define F(x) (uint8_t)(TONE_FREQ / x - 1)

#define NOTE_A4 F(440)
#define NOTE_Ab4 F(466)
#define NOTE_C5 F(523)
#define NOTE_G4 F(392)
#define NOTE_F4 F(349)
#define NOTE_D5 F(587)
#define NOTE_E4 F(330)
#define NOTE_D4 F(294)
#define NOTE_E5 F(659)
#define NOTE_F5 F(698)
#define NOTE_G5 F(784)
#define NOTE_Cb5 F(554)

#define DURATION_0 0
#define DURATION_450 1
#define DURATION_100 2
#define DURATION_200 3
#define DURATION_300 4
#define DURATION_1100 5
#define DURATION_400 6
#define DURATION_50 7
#define DURATION_850 8
#define DURATION_500 9
#define DURATION_150 10
#define DURATION_250 11
#define DURATION_350 12

const uint32_t melody_durations[] = {
  LOOP_CTR_32(MS_TO_CYCLES(0)), // 0
  LOOP_CTR_32(MS_TO_CYCLES(450)), // 1
  LOOP_CTR_32(MS_TO_CYCLES(100)), // 2
  LOOP_CTR_32(MS_TO_CYCLES(200)), // 3
  LOOP_CTR_32(MS_TO_CYCLES(300)), // 4
  LOOP_CTR_32(MS_TO_CYCLES(1100)), // 5
  LOOP_CTR_32(MS_TO_CYCLES(400)), // 6
  LOOP_CTR_32(MS_TO_CYCLES(50)), // 7
  LOOP_CTR_32(MS_TO_CYCLES(850)), // 8
  LOOP_CTR_32(MS_TO_CYCLES(500)), // 9
  LOOP_CTR_32(MS_TO_CYCLES(150)), // 10
  LOOP_CTR_32(MS_TO_CYCLES(250)), // 11
  LOOP_CTR_32(MS_TO_CYCLES(350)), // 12
};

typedef uint16_t melody_data;

const melody_data melody[92] = {
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_150 << 4) | DURATION_100),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_C5 << 8) | (DURATION_50 << 4) | DURATION_300),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_350 << 4) | DURATION_150),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_150 << 4) | DURATION_100),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_300),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_350 << 4) | DURATION_150),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_100 << 4) | DURATION_300),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_100 << 4) | DURATION_50),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_C5 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_350 << 4) | DURATION_100),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_100 << 4) | DURATION_300),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_100 << 4) | DURATION_50),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_50 << 4) | DURATION_150),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_350 << 4) | DURATION_400),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_C5 << 8) | (DURATION_0 << 4) | DURATION_350),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_350 << 4) | DURATION_150),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_100 << 4) | DURATION_250),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_Ab4 << 8) | (DURATION_50 << 4) | DURATION_300),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_350 << 4) | DURATION_150),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_100 << 4) | DURATION_50),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_E4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_350),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_300 << 4) | DURATION_200),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_100 << 4) | DURATION_50),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_100 << 4) | DURATION_50),
  (uint16_t)((NOTE_G4 << 8) | (DURATION_150 << 4) | DURATION_100),
  (uint16_t)((NOTE_F4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_E4 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_D4 << 8) | (DURATION_450 << 4) | DURATION_150),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_50 << 4) | DURATION_350),
  (uint16_t)((NOTE_C5 << 8) | (DURATION_1100 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_200 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_350),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_1100 << 4) | DURATION_100),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_G5 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_0 << 4) | DURATION_350),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_1100 << 4) | DURATION_150),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_100 << 4) | DURATION_150),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_50 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_350),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_1100 << 4) | DURATION_100),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_200 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_50),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_0 << 4) | DURATION_350),
  (uint16_t)((NOTE_C5 << 8) | (DURATION_850 << 4) | DURATION_200),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_150 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_200 << 4) | DURATION_100),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_200 << 4) | DURATION_50),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_1100 << 4) | DURATION_150),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_200 << 4) | DURATION_100),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_150 << 4) | DURATION_100),
  (uint16_t)((NOTE_F5 << 8) | (DURATION_50 << 4) | DURATION_250),
  (uint16_t)((NOTE_E5 << 8) | (DURATION_1100 << 4) | DURATION_150),
  (uint16_t)((NOTE_A4 << 8) | (DURATION_50 << 4) | DURATION_200),
  (uint16_t)((NOTE_Cb5 << 8) | (DURATION_100 << 4) | DURATION_200),
  (uint16_t)((NOTE_D5 << 8) | (DURATION_0 << 4) | DURATION_500),
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) ((melody[x] >> 8) & 0xFf)
#define MELODY_NO_TONE_DURATION(x) melody_durations[(melody[x] >> 4) & 0xf]
#define MELODY_DURATION(x) melody_durations[melody[x] & 0xF]

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