# Musical Melody with PWM Volume Control

Plays melodies ("Twinkle, Twinkle, Little Star") on a passive piezo buzzer using Timer 2 in PWM mode on pin **PA3**, with real-time volume adjustment controlled by an analog potentiometer sampled on pin **PB3** via the internal 8-bit ADC.

> **Hardware note:** Configured for **PFS122** (`board = pfs122`, 16 MHz CPU), which features built-in hardware ADC channels.

## How It Works

- **Tone & Volume via PWM:** Timer 2 is configured in PWM mode on pin **PA3** (`TM2C_MODE_PWM | TM2C_OUT_PA3`). Pitch frequency is adjusted via `TM2S`, and volume is modulated using PWM duty cycle.
- **ADC Volume Input:** Samples analog channel AD3 on pin **PB3** (`ADCC_CH_AD3_PB3`) in real time, mapping potentiometer position to audio volume.
- **Optional Debugging:** Includes software UART output via Timer 2 interrupt for logging ADC readings.

## Wiring

| Pin | Function | Connection |
|:---|:---|:---|
| **PA3** (TM2PWM) | Buzzer Output | Passive Piezo Buzzer (+) pin; (-) pin to GND |
| **PB3** (AD3) | ADC Input | Potentiometer wiper (0V .. VDD) |
| **VDD** | Power | 3.3V–5.0V DC supply |
| **GND** | Ground | Common Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
