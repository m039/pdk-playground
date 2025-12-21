#ifndef _KOMAROVO_O_
#define _KOMAROVO_O_

// https://www.youtube.com/watch?v=G6NqhIYzpZM&list=RDACYB1XNs04w&index=4
// https://www.midi-karaoke.info/215c2f1d.ru.html

#include <stdint.h>
#include "delay.h"

#define TONE_FREQ (16000000 / (2 * 64 * 12))
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
#define DURATION_481 1
#define DURATION_259 2
#define DURATION_37 3
#define DURATION_296 4
#define DURATION_74 5
#define DURATION_333 6
#define DURATION_111 7
#define DURATION_1073 8
#define DURATION_370 9
#define DURATION_851 10
#define DURATION_148 11
#define DURATION_1110 12
#define DURATION_407 13
#define DURATION_185 14
#define DURATION_444 15
#define DURATION_222 16

const uint32_t melody_durations[] = {
  LOOP_CTR_32(MS_TO_CYCLES(0)), // 0
  LOOP_CTR_32(MS_TO_CYCLES(483)), // 1
  LOOP_CTR_32(MS_TO_CYCLES(250)), // 2
  LOOP_CTR_32(MS_TO_CYCLES(33)), // 3
  LOOP_CTR_32(MS_TO_CYCLES(313)), // 4
  LOOP_CTR_32(MS_TO_CYCLES(79)), // 5
  LOOP_CTR_32(MS_TO_CYCLES(337)), // 6
  LOOP_CTR_32(MS_TO_CYCLES(129)), // 7
  LOOP_CTR_32(MS_TO_CYCLES(1079)), // 8
  LOOP_CTR_32(MS_TO_CYCLES(354)), // 9
  LOOP_CTR_32(MS_TO_CYCLES(846)), // 10
  LOOP_CTR_32(MS_TO_CYCLES(138)), // 11
  LOOP_CTR_32(MS_TO_CYCLES(1117)), // 12
  LOOP_CTR_32(MS_TO_CYCLES(421)), // 13
  LOOP_CTR_32(MS_TO_CYCLES(175)), // 14
  LOOP_CTR_32(MS_TO_CYCLES(458)), // 15
  LOOP_CTR_32(MS_TO_CYCLES(217)), // 16
};

typedef uint16_t melody_data;

const melody_data melody[92] = {
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_185),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_148 << 5) | DURATION_74),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_185),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_C5 << 10) | (DURATION_37 << 5) | DURATION_333),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_370 << 5) | DURATION_148),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_37 << 5) | DURATION_185),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_148 << 5) | DURATION_74),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_74 << 5) | DURATION_185),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_37 << 5) | DURATION_296),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_333 << 5) | DURATION_148),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_111 << 5) | DURATION_296),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_185),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_111 << 5) | DURATION_111),
  (uint16_t)((NOTE_C5 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_370 << 5) | DURATION_111),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_296),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_37 << 5) | DURATION_185),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_333 << 5) | DURATION_407),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_74 << 5) | DURATION_148),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_185),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_C5 << 10) | (DURATION_37 << 5) | DURATION_370),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_333 << 5) | DURATION_111),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_111 << 5) | DURATION_259),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_Ab4 << 10) | (DURATION_37 << 5) | DURATION_296),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_333 << 5) | DURATION_185),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_111 << 5) | DURATION_111),
  (uint16_t)((NOTE_E4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_74 << 5) | DURATION_74),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_370),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_259 << 5) | DURATION_185),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_74 << 5) | DURATION_37),
  (uint16_t)((NOTE_G4 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_F4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_E4 << 10) | (DURATION_37 << 5) | DURATION_111),
  (uint16_t)((NOTE_D4 << 10) | (DURATION_444 << 5) | DURATION_148),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_37 << 5) | DURATION_333),
  (uint16_t)((NOTE_C5 << 10) | (DURATION_1110 << 5) | DURATION_111),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_370),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_1110 << 5) | DURATION_111),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_111 << 5) | DURATION_148),
  (uint16_t)((NOTE_G5 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_0 << 5) | DURATION_333),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_1110 << 5) | DURATION_148),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_111 << 5) | DURATION_148),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_333),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_1110 << 5) | DURATION_111),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_74),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_37 << 5) | DURATION_333),
  (uint16_t)((NOTE_C5 << 10) | (DURATION_851 << 5) | DURATION_185),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_148 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_1110 << 5) | DURATION_111),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_185 << 5) | DURATION_74),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_148 << 5) | DURATION_74),
  (uint16_t)((NOTE_F5 << 10) | (DURATION_37 << 5) | DURATION_259),
  (uint16_t)((NOTE_E5 << 10) | (DURATION_1073 << 5) | DURATION_148),
  (uint16_t)((NOTE_A4 << 10) | (DURATION_37 << 5) | DURATION_222),
  (uint16_t)((NOTE_Cb5 << 10) | (DURATION_74 << 5) | DURATION_185),
  (uint16_t)((NOTE_D5 << 10) | (DURATION_0 << 5) | DURATION_481),
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