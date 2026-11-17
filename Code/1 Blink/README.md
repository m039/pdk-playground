# Blink LED (C Language)

Demonstrates basic LED blinking (1 second ON, 1 second OFF) in a software timing loop written in C using SDCC and `pdk-platformio`.

> **Hardware note:** Configured for **PFS154**, but compatible with virtually all Padauk MCUs including **PMS150C**.

## Wiring

| Signal | PFS154 (SOP-16) | PMS150C (SOT23-6) | Connection |
|:---|:---:|:---:|:---|
| **PA4** | Pin 10 | Pin 1 | LED with resistor |
| **VDD** | Pin 5 | Pin 5 | 2.0V–5.5V |
| **GND** | Pin 12 | Pin 2 | Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
