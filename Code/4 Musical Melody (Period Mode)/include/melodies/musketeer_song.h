#ifndef _MUSKETEER_SONG_
#define _MUSKETEER_SONG_

// https://www.youtube.com/watch?v=BCi6h34adNo&list=RDACYB1XNs04w&index=4
// https://www.midi-karaoke.info/217427d5.ru.html

#include <stdint.h>
#include "delay.h"

#define NOTE_E5 1
#define NOTE_D5 2
#define NOTE_C5 3
#define NOTE_E4 4
#define NOTE_A4 5
#define NOTE_Cb5 6
#define NOTE_A5 7
#define NOTE_F5 8
#define NOTE_B4 9

#define TONE_FREQ (16000000 / (2 * 64 * 3))
#define F(x) (uint8_t)(TONE_FREQ / x - 1)
const uint8_t melody_tones[] = {
  0,
  F(659), // E5
  F(587), // D5
  F(523), // C5
  F(330), // E4
  F(440), // A4
  F(554), // Cb5
  F(880), // A5
  F(698), // F5
  F(494), // B4
};

#define DURATION_0 0
#define DURATION_516 1
#define DURATION_266 2
#define DURATION_271 3
#define DURATION_16 4
#define DURATION_21 5
#define DURATION_667 6
#define DURATION_542 7
#define DURATION_1188 8
#define DURATION_422 9
#define DURATION_167 10
#define DURATION_688 11
#define DURATION_563 12
#define DURATION_313 13
#define DURATION_1083 14
#define DURATION_188 15
#define DURATION_63 16
#define DURATION_78 17
#define DURATION_208 18
#define DURATION_2000 19
#define DURATION_83 20
#define DURATION_734 21
#define DURATION_479 22
#define DURATION_484 23
#define DURATION_229 24
#define DURATION_234 25
#define DURATION_500 26
#define DURATION_245 27
#define DURATION_630 28
#define DURATION_120 29
#define DURATION_250 30
#define DURATION_255 31

const uint16_t melody_durations[] = {
  0, // 0
  516, // 1
  266, // 2
  271, // 3
  16, // 4
  21, // 5
  667, // 6
  542, // 7
  1188, // 8
  422, // 9
  167, // 10
  688, // 11
  563, // 12
  313, // 13
  1083, // 14
  188, // 15
  63, // 16
  78, // 17
  208, // 18
  2000, // 19
  83, // 20
  734, // 21
  479, // 22
  484, // 23
  229, // 24
  234, // 25
  500, // 26
  245, // 27
  630, // 28
  120, // 29
  250, // 30
  255, // 31
};

typedef struct {
    uint8_t t;
    uint8_t d;
    uint8_t nd;
} melody_data;

const melody_data melody[27] = {
  {NOTE_E5, DURATION_271, DURATION_229},
  {NOTE_E5, DURATION_255, DURATION_245},
  {NOTE_E5, DURATION_255, DURATION_245},
  {NOTE_E5, DURATION_234, DURATION_266},
  {NOTE_D5, DURATION_188, DURATION_63},
  {NOTE_C5, DURATION_422, DURATION_78},
  {NOTE_E4, DURATION_1083, DURATION_667},
  {NOTE_A4, DURATION_500, DURATION_0},
  {NOTE_C5, DURATION_484, DURATION_16},
  {NOTE_C5, DURATION_688, DURATION_313},
  {NOTE_E5, DURATION_500, DURATION_0},
  {NOTE_A4, DURATION_479, DURATION_21},
  {NOTE_D5, DURATION_1188, DURATION_563},
  {NOTE_E5, DURATION_250, DURATION_250},
  {NOTE_E5, DURATION_271, DURATION_229},
  {NOTE_E5, DURATION_271, DURATION_229},
  {NOTE_E5, DURATION_255, DURATION_245},
  {NOTE_E5, DURATION_484, DURATION_16},
  {NOTE_D5, DURATION_167, DURATION_83},
  {NOTE_C5, DURATION_500, DURATION_0},
  {NOTE_Cb5, DURATION_484, DURATION_16},
  {NOTE_A5, DURATION_734, DURATION_516},
  {NOTE_F5, DURATION_630, DURATION_120},
  {NOTE_E5, DURATION_500, DURATION_0},
  {NOTE_D5, DURATION_542, DURATION_208},
  {NOTE_B4, DURATION_500, DURATION_0},
  {NOTE_C5, DURATION_2000, DURATION_0},
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) melody_tones[melody[x].t]
#define MELODY_NO_TONE_DURATION(x) melody_durations[melody[x].nd]
#define MELODY_DURATION(x) melody_durations[melody[x].d]

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