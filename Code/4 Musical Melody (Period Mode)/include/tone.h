#ifndef _TONE_
#define _TONE_

#include <pdk/device.h>

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

#endif