# PMS150C I2C 1-Bit ADC & Arduino Monitor

Measures supply voltage (VDD) and an analog pin (PA4) in 100 mV steps using the PMS150C internal comparator and resistor ladder, exposing readings over an emulated I2C slave interface.

> **Hardware note:** Tested on **PFS154** during development, but size-optimized to fit into the OTP **PMS150C** (1 KW ROM / 64 B RAM).

Works in tandem with the companion Arduino sketch in **`arduino/TwoWireTest/TwoWireTest.ino`** (I2C master monitor).

## Wiring

| PMS150C (SOP-8) | Arduino Nano | Description |
|:---|:---|:---|
| **PA0** (Pin 1) | **A5** (SCL) | 4.7 kΩ pull-up to VCC required |
| **PA5** (Pin 6) | **A4** (SDA) | 4.7 kΩ pull-up to VCC required |
| **PA4** (Pin 5) | Analog Input | Voltage to measure (0 .. VDD) |
| **VDD** (Pin 4) | **5V** | Power (2.5V–5.5V) |
| **GND** (Pin 2) | **GND** | Ground |

*I2C Speed:* **50 kHz** (`Wire.setClock(50000)`).

## I2C Registers (Address `0x2A`)

| Reg | Name | Size | Value |
|:---:|:---|:---:|:---|
| `0x00` | VDD | 1 B | Supply voltage in 100 mV units (e.g. `50` = 5.0 V) |
| `0x01` | VPA4 | 1 B | PA4 voltage in 100 mV units (e.g. `33` = 3.3 V) |
| `0x02` | UID | 6 B | Unique chip ID from calibration words |

## Usage

1. **Padauk**: Build and upload firmware via PlatformIO (`pio run -t upload`).
2. **Arduino**: Upload `arduino/TwoWireTest/TwoWireTest.ino`, open Serial Monitor at **115200 baud**.

---

*Code comments and README.md thoroughly prepared by Gemini (G).*
