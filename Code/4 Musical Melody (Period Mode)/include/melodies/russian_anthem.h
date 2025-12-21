#ifndef RUSSIAN_ANTHEM
#define RUSSIAN_ANTHEM

#include <stdint.h>
#include "delay.h"

#define NG4 1
#define ND4 2
#define NE4 3
#define NFb4 4
#define NB3 5
#define NC4 6
#define NG3 7
#define NA3 8
#define NA4 9
#define NB4 10
#define NC5 11

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

#define D125 0
#define D250 1
#define D375 2
#define D500 3
#define D750 4
#define D1000 5
#define D1500 6
#define D2000 7
#define D0 8

const uint32_t melody_durations[] = {
    LOOP_CTR_32(MS_TO_CYCLES(125)), // 0
    LOOP_CTR_32(MS_TO_CYCLES(250)), // 1
    LOOP_CTR_32(MS_TO_CYCLES(375)), // 2
    LOOP_CTR_32(MS_TO_CYCLES(500)), // 3
    LOOP_CTR_32(MS_TO_CYCLES(750)), // 4
    LOOP_CTR_32(MS_TO_CYCLES(1000)), // 5
    LOOP_CTR_32(MS_TO_CYCLES(1500)), // 6
    LOOP_CTR_32(MS_TO_CYCLES(2000)), // 7
    LOOP_CTR_32(MS_TO_CYCLES(0)) // 8
};

typedef struct {
    uint8_t t;
    uint8_t d;
} melody_data;

// Generated from https://arduinomidi.netlify.app/

const melody_data melody[102] = {
 {NG4, D1500},
 {ND4, D250},
 {NG4, D500},
 {ND4, D375},
 {NE4, D125},
 {NFb4, D500},
 {NB3, D375},
 {NB3, D125},
 {NE4, D500},
 {ND4, D375},
 {NC4, D125},
 {ND4, D500},
 {NG3, D375},
 {NG3, D125},
 {NA3, D500},
 {NA3, D375},
 {NB3, D125},
 {NC4, D500},
 {NC4, D375},
 {ND4, D125},
 {NE4, D500},
 {NFb4, D375},
 {NG4, D125},
 {NA4, D750},
 {ND4, D250},
 {NB4, D500},
 {NA4, D375},
 {NG4, D125},
 {NA4, D500},
 {NFb4, D375},
 {ND4, D125},
 {NG4, D500},
 {NFb4, D375},
 {NE4, D125},
 {NFb4, D500},
 {NB3, D375},
 {NB3, D125},
 {NE4, D500},
 {ND4, D375},
 {NC4, D125},
 {ND4, D500},
 {NG3, D375},
 {NG3, D125},
 {NG4, D500},
 {NFb4, D375},
 {NE4, D125},
 {ND4, D250},
 {NFb4, D250},
 {NG4, D250},
 {NA4, D250},
 {NB4, D1000},
 {NA4, D250},
 {NG4, D250},
 {NFb4, D250},
 {NG4, D250},
 {NA4, D750},
 {ND4, D250},
 {ND4, D1000},
 {NG4, D1000},
 {NFb4, D250},
 {NE4, D250},
 {ND4, D250},
 {NE4, D250},
 {NFb4, D750},
 {NB3, D250},
 {NB3, D1000},
 {NG4, D500},
 {NE4, D375},
 {NFb4, D125},
 {NG4, D500},
 {NE4, D375},
 {NFb4, D125},
 {NG4, D500},
 {NE4, D375},
 {NG4, D125},
 {NC5, D1000},
 {NC5, D1000},
 {NB4, D250},
 {NA4, D250},
 {NG4, D250},
 {NA4, D250},
 {NB4, D750},
 {NG4, D250},
 {NG4, D1000},
 {NA4, D1000},
 {NG4, D250},
 {NFb4, D250},
 {NE4, D250},
 {NFb4, D250},
 {NG4, D750},
 {NE4, D250},
 {NE4, D1000},
 {NG4, D500},
 {NFb4, D375},
 {NE4, D125},
 {ND4, D500},
 {NG3, D375},
 {NG3, D125},
 {ND4, D1000},
 {NE4, D500},
 {NFb4, D500},
 {NG4, D2000},
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) melody_tones[melody[x].t]
#define MELODY_NO_TONE(x) LOOP_CTR_32(MS_TO_CYCLES(25))
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