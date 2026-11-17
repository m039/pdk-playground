# Musical Melody (Period Mode)

Plays musical melodies (such as "Komarovo") on a passive piezo buzzer using hardware Timer 2 (TM2) in frequency period mode (`TM2C_MODE_PERIOD`). Supports ultra-low-power sleep with button wake-up.

> **Hardware note:** Includes multi-target configurations in `platformio.ini` for **PFS154**, **PMS150G**, **PFS122**, and **PMS150C**.

## How It Works

- **Tone Generation:** Uses Timer 2 in Period Mode (`TM2C_MODE_PERIOD | TM2C_OUT_PA3`) on pin **PA3** to generate square waves corresponding to musical notes.
- **Button Control:** Monitors a pushbutton on **PA4** (with internal pull-up `PAPH`).
- **Low-Power Sleep:** When playback finishes, the MCU enters deep sleep (`stopsys`), consuming minimal quiescent current until the button is pressed again.

## Wiring

| Signal | PFS154 (SOP-16) | PMS150C / PMS150G (SOT23-6) | PFS122 (SOP-16) | Connection |
|:---|:---:|:---:|:---:|:---|
| **PA3** (`TM2PWM`) | Pin 9 | Pin 6 | Pin 13 | Piezo buzzer (+) |
| **PA4** | Pin 10 | Pin 1 | Pin 14 | Pushbutton to GND |
| **VDD** | Pin 5 | Pin 5 | Pin 1 | 2.0V–5.5V |
| **GND** | Pin 12 | Pin 2 | Pin 16 | Ground |

## Usage

Build and upload firmware for your specific MCU target:
```bash
# Upload to PFS154
pio run -e pfs154 -t upload

# Upload to PMS150C
pio run -e pms150c -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
