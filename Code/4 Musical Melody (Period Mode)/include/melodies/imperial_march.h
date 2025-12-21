#ifndef _IMPERIAL_MARCH_
#define _IMPERIAL_MARCH_

// https://medium.com/@bigmaitz/using-arduino-to-play-star-wars-imperial-march-tune-47f75a6e1e43

#include <stdint.h>
#include "delay.h"

#define TONE_FREQ (16000000 / (2 * 64 * 2))
#define F(x) (uint8_t)(TONE_FREQ / x - 1)

#define REST  0
#define Nd    1
#define Ne    2
#define Nf    3
#define Ng    4
#define NgS   5
#define Na    6
#define NaS   7
#define Nb    8
#define NcH   9
#define NcSH  10
#define NdH   11
#define NdSH  12
#define NeH   13
#define NfH   14
#define NfSH  15
#define NgH   16
#define NgSH  17
#define NaH   18
#define Nc    10

const uint8_t melody_tones[] = {
  0,
  F(294), // Nd
  F(329), // Ne
  F(349), // Nf
  F(391), // Ng
  F(415), // NgS // G Sharp
  F(440), // Na
  F(455), // NaS // A Sharp
  F(466), // Nb
  F(523), // NcH // C High
  F(554), // NcSH // C Sharp High
  F(587), // NdH // D High
  F(622), // NdSH // D Sharp High
  F(659), // NeH // E High
  F(698), // NfH // F High
  F(740), // NfSH // F Sharp High
  F(784), // NgH // G High
  F(830), // NgSH // G Sharp High
  F(880), // NaH // A High
  F(261), // Nc
};

#define D650 0
#define D500 1
#define D375 2
#define D350 3
#define D325 4
#define D300 5
#define D250 6
#define D175 7
#define D150 8
#define D125 9

const uint16_t melody_durations[] = {
 650, // 0
 500, // 1
 375, // 2
 350, // 3
 325, // 4
 300, // 5
 250, // 6
 175, // 7
 150, // 8
 125 // 9
};

typedef struct {
    uint8_t t;
    uint8_t d;
} melody_data;

const melody_data melody[] = {
  {Na, D500}, {Na, D500}, {Na, D500}, {Nf, D350}, {NcH, D150}, {Na, D500}, {Nf, D350}, {NcH, D150}, {Na, D650}, {REST, D500}, // first
  {NeH, D500}, {NeH, D500}, {NeH, D500}, {NfH, D350}, {NcH, D150}, {NgS, D500}, {Nf, D350}, {NcH, D150}, {Na, D650}, {REST, D500},
  
  {NaH, D500}, {Na, D300}, {Na, D150}, {NaH, D500}, {NgSH, D325}, {NgH, D175}, {NfSH, D125}, {NfH, D125}, {NfSH, D250}, {REST, D500}, // second
  {NaS, D250}, {NdSH, D500}, {NdH, D325}, {NcSH, D175}, {NcH, D125}, {Nb, D125}, {NcH, D250}, {REST, D500},

  {Nf, D250}, {NgS, D500}, {Nf, D350}, {Na, D125}, {NcH, D500}, {Na, D375}, {NcH, D125}, {NeH, D650}, {REST, D500}, // variant 1

  {NaH, D500}, {Na, D300}, {Na, D150}, {NaH, D500}, {NgSH, D325}, {NgH, D175}, {NfSH, D125}, {NfH, D125}, {NfSH, D250}, {REST, D500}, // second
  {NaS, D250}, {NdSH, D500}, {NdH, D325}, {NcSH, D175}, {NcH, D125}, {Nb, D125}, {NcH, D250}, {REST, D500},

  {Nf, D250}, {NgS, D500}, {Nf, D350}, {NcH, D125}, {Na, D500}, {Nf, D375}, {NcH, D125}, {Na, D650}, {REST, D500} // variant 2
};

#define MELODY_SIZE sizeof(melody) / sizeof(melody_data)
#define MELODY_TONE(x) melody_tones[melody[x].t]
#define MELODY_NO_TONE_DURATION(x) 25))
#define MELODY_DURATION(x) melody_durations[melody[x].d]

void tone(uint8_t frequency)
{
  if (frequency <= 0) {
    TM2B = 0;
  } else {
    TM2S = 3 << 5 | 1;
    TM2B = frequency;
  }
}

#endif