# PMS150C I2C 1-Bit ADC & Arduino Monitor

Measures supply voltage (VDD) and an analog pin (PA4) using the PMS150C internal comparator with internal resistor ladder (Case 4), exposing readings over an emulated I2C slave interface.

> **Hardware note:** Tested on **PFS154** during development, but size-optimized to fit into the OTP **PMS150C** (1 KW ROM / 64 B RAM). The microcontroller currently responds reliably at **8 MHz**; it does not work at lower clock frequencies.

Works in tandem with the companion Arduino sketch in **`arduino/TwoWireTest/TwoWireTest.ino`** (I2C master monitor).

## Comparator Case 4 & Measurable Values

The comparator uses **Case 4** with internal resistor ladder reference:
$$V_{INT\_R} = \frac{N + 1}{32} \times V_{DD} \quad (N = 0 \dots 15)$$

> *Note:* For complete comparator specifications and alternate cases, refer to the Padauk PMS150C datasheet.

### Returned Values:

- **VDD (`get_vdd()`, Register `0x00`)**:
  - Measurable range: **2.40 V to 5.49 V**
  - Discrete returned values (in 100 mV units): `24`, `25`, `27`, `29`, `32`, `34`, `38`, `42`, `48`, `54`

- **PA4 Analog Pin (`get_vpa4()`, Register `0x01`)**:
  - Measurable range: **$0.031 \times V_{DD}$ to $0.50 \times V_{DD}$** (up to half of $V_{DD}$)
    - At $V_{DD} = 5.0\text{ V}$: **0.16 V to 2.50 V** (values `1` to `25` in 100 mV units)
    - At $V_{DD} = 3.3\text{ V}$: **0.10 V to 1.65 V** (values `1` to `16` in 100 mV units)

## Wiring (PMS150C / PFS154 to Arduino Nano)

| Signal | PMS150C-U06 (SOT23-6) | PMS150C (SOP-8) | PFS154 (SOP-16) | Arduino Nano | Notes |
|:---|:---:|:---:|:---:|:---|:---|
| **PA6** (`SCL`) | Pin 3 | Pin 3 | Pin 7 | **A5** | 4.7 kΩ pull-up to VCC |
| **PA3** (`SDA`) | Pin 6 | Pin 5 | Pin 9 | **A4** | 4.7 kΩ pull-up to VCC |
| **PA4** (`CIN+`) | Pin 1 | Pin 6 | Pin 10 | Analog Input | Measured voltage ($0.03 \dots 0.50 \times V_{DD}$) |
| **VDD** | Pin 5 | Pin 1 | Pin 5 | **5V** (or 3V3) | 2.4V–5.5V |
| **GND** | Pin 2 | Pin 8 | Pin 12 | **GND** | Ground |

*I2C Speed:* **50 kHz** (`Wire.setClock(50000)`).  
*CPU Clock:* **8 MHz** (lower clock speeds do not work).

## I2C Registers (Address `0x2A`)

| Reg | Name | Size | Value |
|:---:|:---|:---:|:---|
| `0x00` | VDD | 1 B | Supply voltage in 100 mV units (24 to 54; `0` = out of range) |
| `0x01` | VPA4 | 1 B | PA4 voltage in 100 mV units (up to $0.5 \times V_{DD}$; `0` = out of range) |
| `0x02` | UID | 6 B | Unique chip ID from calibration words |

## Usage

1. **Padauk**: Build and upload firmware via PlatformIO (`pio run -t upload`).
2. **Arduino**: Upload `arduino/TwoWireTest/TwoWireTest.ino`, open Serial Monitor at **115200 baud**.

---

*Code comments and README.md thoroughly prepared by Gemini (G).*
