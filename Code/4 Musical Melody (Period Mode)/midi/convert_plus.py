input_notes = """
#define A4 440
#define Ab4 466
#define C5 523
#define G4 392
#define F4 349
#define D5 587
#define E4 330
#define D4 294
#define E5 659
#define F5 698
#define G5 784
#define Cb5 554
"""

input_melody = """
 {A4, 50, 58},
 {A4, 63, 71},
 {A4, 200, 50},
 {A4, 75, 158},
 {Ab4, 188, 67},
 {A4, 83, 54},
 {C5, 325, 38},
 {Ab4, 133, 371},
 {Ab4, 67, 67},
 {A4, 83, 50},
 {G4, 200, 50},
 {G4, 83, 158},
 {F4, 192, 58},
 {G4, 83, 54},
 {Ab4, 313, 50},
 {A4, 146, 333},
 {A4, 63, 63},
 {A4, 67, 63},
 {A4, 300, 104},
 {A4, 63, 83},
 {Ab4, 171, 75},
 {A4, 129, 108},
 {C5, 225, 37},
 {Ab4, 117, 367},
 {A4, 54, 71},
 {Ab4, 63, 71},
 {A4, 283, 83},
 {G4, 58, 75},
 {G4, 171, 46},
 {D5, 421, 350},
 {A4, 79, 46},
 {A4, 83, 46},
 {A4, 208, 46},
 {A4, 146, 79},
 {Ab4, 192, 63},
 {A4, 87, 33},
 {C5, 363, 25},
 {Ab4, 129, 346},
 {Ab4, 75, 58},
 {A4, 79, 54},
 {G4, 271, 96},
 {G4, 71, 67},
 {F4, 221, 42},
 {G4, 75, 46},
 {Ab4, 313, 42},
 {A4, 171, 333},
 {F4, 54, 71},
 {F4, 54, 79},
 {F4, 213, 29},
 {F4, 129, 125},
 {E4, 212, 29},
 {F4, 67, 63},
 {A4, 371, 29},
 {G4, 196, 275},
 {G4, 54, 83},
 {G4, 46, 92},
 {G4, 75, 171},
 {F4, 208, 42},
 {E4, 96, 37},
 {D4, 158, 458},
 {D5, 212, 50},
 {E5, 87, 38},
 {D5, 337, 38},
 {C5, 117, 1108},
 {E5, 79, 183},
 {E5, 67, 54},
 {E5, 354, 38},
 {F5, 121, 1117},
 {F5, 137, 125},
 {G5, 87, 38},
 {F5, 346, 17},
 {E5, 158, 1092},
 {E5, 137, 100},
 {F5, 87, 42},
 {E5, 346, 38},
 {D5, 125, 1113},
 {D5, 92, 183},
 {E5, 71, 50},
 {D5, 337, 21},
 {C5, 183, 846},
 {E5, 79, 163},
 {E5, 75, 175},
 {E5, 67, 179},
 {E5, 225, 33},
 {F5, 129, 1117},
 {F5, 79, 188},
 {F5, 75, 150},
 {F5, 250, 29},
 {E5, 138, 1079},
 {A4, 217, 33},
 {Cb5, 175, 79},
 {D5, 483, 0},
 """

durations = set({})
notes = {}
melodies = []
not_rounded = {}

def rount_duration(x):
    base = 37
    return int(round(x / base) * base)

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
    
    d = rount_duration(int(a[1]))
    nd = rount_duration(int(a[2]))

    not_rounded[d] = int(a[1])
    not_rounded[nd] = int(a[2])

    durations.add(d)
    durations.add(nd)

    melodies.append(f"(uint16_t)((NOTE_{a[0]} << 10) | (DURATION_{nd} << 5) | DURATION_{d}),")

print("""
#include <stdint.h>
#include "delay.h"
""")

notes_list = []

for key, value in notes.items():
    notes_list.append([key, value])

print("#define TONE_FREQ (16000000 / (2 * 64 * 12))")
print("#define F(x) (uint8_t)(TONE_FREQ / x - 1)")

print()

for i, n in enumerate(notes_list):
    print(f"#define NOTE_{n[0]} F({n[1]})")

print()

durations = list(durations)

for i, d in enumerate(durations):
    print(f"#define DURATION_{d} {i}")

print()

print("const uint32_t melody_durations[] = {")

for i, d in enumerate(durations):
    print(f"  LOOP_CTR_32(MS_TO_CYCLES({not_rounded[d]})), // {i}")

print("};")

print("""  
typedef uint16_t melody_data;
""")

print(f"const melody_data melody[{len(melodies)}] = {{")

for i, m in enumerate(melodies):
    print(f"  {m}")

print("};")

print("""
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
""")