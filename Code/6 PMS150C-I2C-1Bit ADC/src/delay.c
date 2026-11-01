#include "delay.h"

// 3 cycles per loop, 7 cycles overhead
void _delay_loop_8(uint8_t loop_ctr) {
  loop_ctr; // Ignore unreferenced function argument warning
__asm
00001$:
  dzsn   __delay_loop_8_PARM_1
    goto 00001$
__endasm;
}

#if 0
// Unused 16-bit and 32-bit delay loops disabled to save ROM/RAM on PMS150C
void _delay_loop_16(uint16_t loop_ctr) { (void)loop_ctr; }
void _delay_loop_32(uint32_t loop_ctr) { (void)loop_ctr; }
#endif