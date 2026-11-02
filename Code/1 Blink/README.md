# Blink LED (C Language)

Demonstrates basic LED blinking (1 second ON, 1 second OFF) in a software timing loop written in C using SDCC and `pdk-platformio`.

> **Hardware note:** Configured for **PFS154**, but compatible with virtually all Padauk MCUs including **PMS150C**.

## Hardware Setup

The LED is connected in an **active-LOW (current-sink)** configuration on pin **PA4**. The microcontroller pulls the pin LOW to light the LED and HIGH (or input) to turn it off.

## Wiring

| Pin | Function | Connection |
|:---|:---|:---|
| **PA4** | LED Sink | Connect to LED Cathode (negative leg); Anode connects to VDD through a 1 kΩ resistor |
| **VDD** | Power | 2.0V–5.5V DC supply |
| **GND** | Ground | Common Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
