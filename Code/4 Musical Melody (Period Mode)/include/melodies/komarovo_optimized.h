#ifndef _KOMAROVO_O_
#define _KOMAROVO_O_

// https://www.youtube.com/watch?v=G6NqhIYzpZM&list=RDACYB1XNs04w&index=4
// https://www.midi-karaoke.info/215c2f1d.ru.html

#include <stdint.h>
#include "delay.h"

#define TONE_FREQ (16000000 / (2 * 64 * 12))
#define F(x) (TONE_FREQ / x - 1)

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
#define DURATION_135 1
#define DURATION_270 2
#define DURATION_15 3
#define DURATION_150 4
#define DURATION_285 5
#define DURATION_30 6
#define DURATION_420 7
#define DURATION_165 8
#define DURATION_300 9
#define DURATION_45 10
#define DURATION_180 11
#define DURATION_1080 12
#define DURATION_315 13
#define DURATION_60 14
#define DURATION_195 15
#define DURATION_1095 16
#define DURATION_840 17
#define DURATION_330 18
#define DURATION_75 19
#define DURATION_465 20
#define DURATION_210 21
#define DURATION_1110 22
#define DURATION_345 23
#define DURATION_90 24
#define DURATION_480 25
#define DURATION_225 26
#define DURATION_360 27
#define DURATION_105 28
#define DURATION_375 29
#define DURATION_120 30
#define DURATION_255 31

const uint16_t melody_durations[] = {
  0, // 0
  138, // 1
  275, // 2
  21, // 3
  150, // 4
  283, // 5
  33, // 6
  421, // 7
  163, // 8
  300, // 9
  50, // 10
  175, // 11
  1079, // 12
  313, // 13
  67, // 14
  188, // 15
  1092, // 16
  846, // 17
  337, // 18
  79, // 19
  458, // 20
  217, // 21
  1117, // 22
  346, // 23
  92, // 24
  483, // 25
  225, // 26
  354, // 27
  100, // 28
  371, // 29
  125, // 30
  250, // 31
};

typedef uint16_t melody_data;

const melody_data melody[92] = {
  (NOTE_A4 << 10) | (DURATION_60 << 5) | DURATION_45,
  (NOTE_A4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_45 << 5) | DURATION_195,
  (NOTE_A4 << 10) | (DURATION_165 << 5) | DURATION_75,
  (NOTE_Ab4 << 10) | (DURATION_60 << 5) | DURATION_195,
  (NOTE_A4 << 10) | (DURATION_60 << 5) | DURATION_90,
  (NOTE_C5 << 10) | (DURATION_45 << 5) | DURATION_330,
  (NOTE_Ab4 << 10) | (DURATION_375 << 5) | DURATION_135,
  (NOTE_Ab4 << 10) | (DURATION_60 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_45 << 5) | DURATION_90,
  (NOTE_G4 << 10) | (DURATION_45 << 5) | DURATION_195,
  (NOTE_G4 << 10) | (DURATION_165 << 5) | DURATION_90,
  (NOTE_F4 << 10) | (DURATION_60 << 5) | DURATION_195,
  (NOTE_G4 << 10) | (DURATION_60 << 5) | DURATION_90,
  (NOTE_Ab4 << 10) | (DURATION_45 << 5) | DURATION_315,
  (NOTE_A4 << 10) | (DURATION_330 << 5) | DURATION_150,
  (NOTE_A4 << 10) | (DURATION_60 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_60 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_105 << 5) | DURATION_300,
  (NOTE_A4 << 10) | (DURATION_90 << 5) | DURATION_60,
  (NOTE_Ab4 << 10) | (DURATION_75 << 5) | DURATION_165,
  (NOTE_A4 << 10) | (DURATION_105 << 5) | DURATION_135,
  (NOTE_C5 << 10) | (DURATION_30 << 5) | DURATION_225,
  (NOTE_Ab4 << 10) | (DURATION_360 << 5) | DURATION_120,
  (NOTE_A4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_Ab4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_90 << 5) | DURATION_285,
  (NOTE_G4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_G4 << 10) | (DURATION_45 << 5) | DURATION_165,
  (NOTE_D5 << 10) | (DURATION_345 << 5) | DURATION_420,
  (NOTE_A4 << 10) | (DURATION_45 << 5) | DURATION_75,
  (NOTE_A4 << 10) | (DURATION_45 << 5) | DURATION_90,
  (NOTE_A4 << 10) | (DURATION_45 << 5) | DURATION_210,
  (NOTE_A4 << 10) | (DURATION_75 << 5) | DURATION_150,
  (NOTE_Ab4 << 10) | (DURATION_60 << 5) | DURATION_195,
  (NOTE_A4 << 10) | (DURATION_30 << 5) | DURATION_90,
  (NOTE_C5 << 10) | (DURATION_30 << 5) | DURATION_360,
  (NOTE_Ab4 << 10) | (DURATION_345 << 5) | DURATION_135,
  (NOTE_Ab4 << 10) | (DURATION_60 << 5) | DURATION_75,
  (NOTE_A4 << 10) | (DURATION_60 << 5) | DURATION_75,
  (NOTE_G4 << 10) | (DURATION_90 << 5) | DURATION_270,
  (NOTE_G4 << 10) | (DURATION_60 << 5) | DURATION_75,
  (NOTE_F4 << 10) | (DURATION_45 << 5) | DURATION_225,
  (NOTE_G4 << 10) | (DURATION_45 << 5) | DURATION_75,
  (NOTE_Ab4 << 10) | (DURATION_45 << 5) | DURATION_315,
  (NOTE_A4 << 10) | (DURATION_330 << 5) | DURATION_165,
  (NOTE_F4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_F4 << 10) | (DURATION_75 << 5) | DURATION_60,
  (NOTE_F4 << 10) | (DURATION_30 << 5) | DURATION_210,
  (NOTE_F4 << 10) | (DURATION_120 << 5) | DURATION_135,
  (NOTE_E4 << 10) | (DURATION_30 << 5) | DURATION_210,
  (NOTE_F4 << 10) | (DURATION_60 << 5) | DURATION_60,
  (NOTE_A4 << 10) | (DURATION_30 << 5) | DURATION_375,
  (NOTE_G4 << 10) | (DURATION_270 << 5) | DURATION_195,
  (NOTE_G4 << 10) | (DURATION_90 << 5) | DURATION_60,
  (NOTE_G4 << 10) | (DURATION_90 << 5) | DURATION_45,
  (NOTE_G4 << 10) | (DURATION_165 << 5) | DURATION_75,
  (NOTE_F4 << 10) | (DURATION_45 << 5) | DURATION_210,
  (NOTE_E4 << 10) | (DURATION_30 << 5) | DURATION_90,
  (NOTE_D4 << 10) | (DURATION_465 << 5) | DURATION_165,
  (NOTE_D5 << 10) | (DURATION_45 << 5) | DURATION_210,
  (NOTE_E5 << 10) | (DURATION_45 << 5) | DURATION_90,
  (NOTE_D5 << 10) | (DURATION_45 << 5) | DURATION_330,
  (NOTE_C5 << 10) | (DURATION_1110 << 5) | DURATION_120,
  (NOTE_E5 << 10) | (DURATION_180 << 5) | DURATION_75,
  (NOTE_E5 << 10) | (DURATION_60 << 5) | DURATION_60,
  (NOTE_E5 << 10) | (DURATION_45 << 5) | DURATION_360,
  (NOTE_F5 << 10) | (DURATION_1110 << 5) | DURATION_120,
  (NOTE_F5 << 10) | (DURATION_120 << 5) | DURATION_135,
  (NOTE_G5 << 10) | (DURATION_45 << 5) | DURATION_90,
  (NOTE_F5 << 10) | (DURATION_15 << 5) | DURATION_345,
  (NOTE_E5 << 10) | (DURATION_1095 << 5) | DURATION_165,
  (NOTE_E5 << 10) | (DURATION_105 << 5) | DURATION_135,
  (NOTE_F5 << 10) | (DURATION_45 << 5) | DURATION_90,
  (NOTE_E5 << 10) | (DURATION_45 << 5) | DURATION_345,
  (NOTE_D5 << 10) | (DURATION_1110 << 5) | DURATION_120,
  (NOTE_D5 << 10) | (DURATION_180 << 5) | DURATION_90,
  (NOTE_E5 << 10) | (DURATION_45 << 5) | DURATION_75,
  (NOTE_D5 << 10) | (DURATION_15 << 5) | DURATION_330,
  (NOTE_C5 << 10) | (DURATION_840 << 5) | DURATION_180,
  (NOTE_E5 << 10) | (DURATION_165 << 5) | DURATION_75,
  (NOTE_E5 << 10) | (DURATION_180 << 5) | DURATION_75,
  (NOTE_E5 << 10) | (DURATION_180 << 5) | DURATION_60,
  (NOTE_E5 << 10) | (DURATION_30 << 5) | DURATION_225,
  (NOTE_F5 << 10) | (DURATION_1110 << 5) | DURATION_135,
  (NOTE_F5 << 10) | (DURATION_195 << 5) | DURATION_75,
  (NOTE_F5 << 10) | (DURATION_150 << 5) | DURATION_75,
  (NOTE_F5 << 10) | (DURATION_30 << 5) | DURATION_255,
  (NOTE_E5 << 10) | (DURATION_1080 << 5) | DURATION_135,
  (NOTE_A4 << 10) | (DURATION_30 << 5) | DURATION_210,
  (NOTE_Cb5 << 10) | (DURATION_75 << 5) | DURATION_180,
  (NOTE_D5 << 10) | (DURATION_0 << 5) | DURATION_480,
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) ((melody[x] >> 10) & 0x3f)
#define MELODY_NO_TONE_DURATION(x) melody_durations[(melody[x] >> 5) & 0x1f]
#define MELODY_DURATION(x) melody_durations[melody[x] & 0x1F]

void tone(uint8_t frequency)
{
  if (frequency <= 0) {
    TM2B = 0;
  } else {
    TM2S = 3 << 5 | (12 - 1);
    TM2B = frequency;
  }
}

#endif