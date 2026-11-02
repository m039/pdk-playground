# LED PWM Fading

Smoothly fades an LED in and out by varying the duty cycle of hardware Timer 2 (TM2) in 8-bit PWM mode on pin **PA3**.

> **Hardware note:** Configured for **PFS154**, compatible with other Padauk MCUs featuring Timer 2 PWM (e.g. **PMS150C**, **PMS150G**).

## How It Works

- Configures Timer 2 in PWM mode (`TM2C_MODE_PWM | TM2C_OUT_PA3`) driven by the internal IHRC clock.
- Inverts output polarity (`TM2C_INVERT_OUT`) to match active-low sink wiring.
- Periodically ramps PWM duty cycle register `TM2B` from 0 to 255 and back down to 0 in steps of 5 with 30 ms intervals.

## Wiring

| Pin | Function | Connection |
|:---|:---|:---|
| **PA3** (TM2PWM) | PWM Output | Connect to LED via 330 Ω / 1 kΩ current-limiting resistor |
| **VDD** | Power | 2.0V–5.5V DC supply |
| **GND** | Ground | Common Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
