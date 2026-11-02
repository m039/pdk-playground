# Blink LED (C Language)

Demonstrates basic LED blinking (1 second ON, 1 second OFF) in a software timing loop written in C using SDCC and `pdk-platformio`.

> **Hardware note:** Configured for **PFS154**, but compatible with virtually all Padauk MCUs including **PMS150C**.

## Wiring

| Pin | Function | Connection |
|:---|:---|:---|
| **PA4** | LED | Connect to LED through a current-limiting resistor |
| **VDD** | Power | 2.0V–5.5V DC supply |
| **GND** | Ground | Common Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
