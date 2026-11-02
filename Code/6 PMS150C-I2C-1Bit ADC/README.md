# PMS150C I2C 1-Bit ADC & Arduino Monitor

This project demonstrates using the ultra-low-cost **Padauk PMS150C** microcontroller as an **I2C slave peripheral** that measures analog voltages using its internal comparator and internal resistor ladder as a 1-bit successive ADC.

> **Important**: This project consists of **two companion firmwares that work together**:
> 1. **Padauk PMS150C Firmware** (in `src/`): Implements the 1-bit ADC engine, calibration reading, and a bit-banged I2C slave interface.
> 2. **Arduino Master Sketch** (in `arduino/TwoWireTest/`): Acts as the I2C master (e.g., on an Arduino Nano/Uno), polls the registers periodically, and displays formatted telemetry via the Serial Monitor.

---

## How It Works

- **1-Bit ADC**: The PMS150C lacks a dedicated SAR ADC peripheral. Instead, it utilizes its internal analog comparator (`comp_compare()`) and internal reference voltage divider ladder to test successive voltage thresholds in **100 mV increments** (from 0 mV up to VDD).
- **Channels**:
  - **VDD**: Supply rail voltage measurement.
  - **PA4**: External analog input pin.
- **Unique ID**: Reads 6 factory calibration/ID bytes from the upper calibration words in ROM.
- **I2C Slave**: Emulates an I2C slave on pins PA0 (SCL) and PA5 (SDA) at 7-bit address **`0x2A`**.

---

## Hardware Connection / Wiring

Connect the Padauk PMS150C (SOP-8 package) to the Arduino (e.g. Arduino Nano) as follows:

| PMS150C Pin | Function | Arduino Nano Pin | Notes |
|:---:|:---:|:---:|:---|
| **Pin 1** (PA0) | SCL | **A5** (SCL) | Needs 4.7 kΩ pull-up resistor to VCC |
| **Pin 2** (GND) | Ground | **GND** | Common ground connection |
| **Pin 4** (VDD) | Power (2.5V–5.5V) | **5V** (or 3.3V) | Decouple with 100 nF ceramic capacitor |
| **Pin 5** (PA4) | Analog In | Voltage Source | 0 to VDD input to measure |
| **Pin 6** (PA5) | SDA | **A4** (SDA) | Needs 4.7 kΩ pull-up resistor to VCC |

> **Pull-up Resistors**: Because PMS150C emulates open-drain I2C pins in software, external **4.7 kΩ pull-up resistors** on both SCL and SDA lines to VCC are required.
>
> **I2C Clock Speed**: Software I2C on an 8 MHz PMS150C works reliably at **50 kHz**. The Arduino companion sketch sets `Wire.setClock(50000)`.

---

## I2C Protocol & Register Map

- **Slave Address**: `0x2A` (7-bit)
- **Register Read Transaction**: Master writes 1 byte (Register Address), sends repeated START, and reads data bytes.

| Register Address | Name | Length | Description |
|:---:|:---:|:---:|:---|
| `0x00` | **VDD** | 1 byte | Supply voltage in units of 100 mV (e.g., `50` = 5.0 V) |
| `0x01` | **VPA4** | 1 byte | Voltage on PA4 in units of 100 mV (e.g., `33` = 3.3 V) |
| `0x02` | **UID** | 6 bytes | 6-byte Unique Chip ID read from calibration words |

---

## Getting Started

### 1. Flash Padauk PMS150C
Build and upload using PlatformIO with the [pdk-platformio](https://github.com/m039/pdk-platformio) environment:
```bash
pio run -t upload
```

### 2. Flash Arduino Monitor
1. Open `arduino/TwoWireTest/TwoWireTest.ino` in the Arduino IDE.
2. Select your board (e.g. **Arduino Nano**, ATmega328P).
3. Upload the sketch and open the **Serial Monitor** at **115200 baud**.
4. You will see startup banners and live readings:
   ```text
   --- PMS150C I2C ADC Monitor ---
   [#1] VDD: 5.00 V (5000 mV, raw: 0x32) | PA4: 3.30 V (3300 mV, raw: 0x21) | UID: 1A 2B 3C 4D 5E 6F
   ```
