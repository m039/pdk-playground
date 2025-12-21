input_notes = """
#define F4 349
#define A4 440
#define D5 587
#define E5 659
#define Cb5 554
#define F5 698
#define C5 523
#define B4 494
#define G5 784
#define A5 880
#define Gb4 415
#define E4 330
"""

input_melody = """
{F4, 91, 76},
 {F4, 95, 72},
 {F4, 94, 73},
 {A4, 264, 70},
 {D5, 103, 64},
 {E5, 272, 61},
 {D5, 106, 60},
 {Cb5, 276, 57},
 {D5, 118, 49},
 {F4, 91, 76},
 {F4, 95, 72},
 {F4, 94, 73},
 {A4, 264, 70},
 {D5, 103, 64},
 {E5, 272, 61},
 {D5, 106, 60},
 {Cb5, 276, 57},
 {D5, 118, 49},
 {F5, 94, 73},
 {F5, 95, 72},
 {F5, 100, 67},
 {E5, 267, 67},
 {D5, 106, 60},
 {C5, 255, 78},
 {B4, 110, 56},
 {A4, 257, 76},
 {B4, 116, 51},
 {C5, 326, 174},
 {C5, 367, 133},
 {E5, 355, 145},
 {G5, 339, 161},
 {A5, 244, 90},
 {G5, 118, 49},
 {A5, 223, 110},
 {G5, 108, 58},
 {A5, 228, 105},
 {G5, 102, 65},
 {E5, 252, 81},
 {C5, 126, 41},
 {D5, 246, 88},
 {C5, 99, 68},
 {D5, 307, 26},
 {C5, 374, 126},
 {C5, 109, 57},
 {C5, 244, 90},
 {C5, 114, 53},
 {F5, 236, 97},
 {E5, 117, 50},
 {Cb5, 252, 81},
 {A4, 166, 1},
 {F5, 252, 81},
 {E5, 103, 64},
 {Cb5, 272, 61},
 {A4, 121, 46},
 {D5, 991, 343},
 {C5, 102, 65},
 {C5, 214, 120},
 {C5, 107, 59},
 {C5, 236, 97},
 {A4, 110, 56},
 {C5, 264, 49},
 {B4, 465, 56},
 {B4, 123, 44},
 {Gb4, 252, 81},
 {E4, 145, 22},
 {D5, 238, 75},
 {B4, 126, 61},
 {D5, 280, 53},
 {C5, 438, 63},
 {C5, 155, 11},
 {D5, 242, 92},
 {E5, 118, 49},
 {G5, 261, 72},
 {F5, 138, 29},
 {F5, 272, 61},
 {E5, 130, 36},
 {E5, 252, 81},
 {D5, 143, 24},
 {C5, 246, 87},
 {B4, 133, 33},
 {E5, 1070, 264},
 {C5, 139, 28},
 {D5, 232, 101},
 {E5, 114, 53},
 {G5, 217, 117},
 {F5, 96, 71},
 {F5, 302, 31},
 {E5, 126, 41},
 {E5, 295, 39},
 {D5, 138, 29},
 {C5, 278, 55},
 {B4, 154, 12},
 {C5, 905, 0},
 """

durations = set({})
notes = {}
melodies = []

for n in input_notes.split("\n"):
    n = n.split(" ")
    if len(n) != 3:
        continue

    notes[n[1]] = int(n[2])

for a in input_melody.split("\n"):
    a = a.replace("{", "")
    a = a.replace("}", "")
    a = a.replace(",", "")
    a = a.strip().split(" ")
    if len(a) != 3:
        continue
    
    d = int(a[1])
    nd = int(a[2])

    durations.add(d)
    durations.add(nd)

    melodies.append(f"{{NOTE_{a[0]}, DURATION_{a[1]}, DURATION_{a[2]}}},")

print("""
#include <stdint.h>
#include "delay.h"
""")

notes_list = []

for key, value in notes.items():
    notes_list.append([key, value])

for i, n in enumerate(notes_list):
    print(f"#define NOTE_{n[0]} {i + 1}")

print("")

print("#define TONE_FREQ (16000000 / (2 * 64 * 3))")
print("#define F(x) (uint8_t)(TONE_FREQ / x - 1)")

print("const uint8_t melody_tones[] = {")
print("  0,")

for i, n in enumerate(notes_list):
    print(f"  F({n[1]}), // {n[0]}")

print("};")

print()

durations = list(durations)

for i, d in enumerate(durations):
    print(f"#define DURATION_{d} {i}")

print()

print("const uint32_t melody_durations[] = {")

for i, d in enumerate(durations):
    print(f"  LOOP_CTR_32(MS_TO_CYCLES({d})), // {i}")

print("};")

print("""  
typedef struct {
    uint8_t t;
    uint8_t d;
    uint8_t nd;
} melody_data;
""")

print(f"const melody_data melody[{len(melodies)}] = {{")

for i, m in enumerate(melodies):
    print(f"  {m}")

print("};")

print("""
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
""")