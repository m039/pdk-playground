# PMS150C I2C 1-Bit ADC & Arduino Monitor

Measures supply voltage (VDD) and an analog pin (PA4) using the PMS150C internal comparator with internal resistor ladder (Case 4), exposing readings over an emulated I2C slave interface.

> **Hardware note:** Tested on **PFS154** during development, but size-optimized to fit into the OTP **PMS150C** (1 KW ROM / 64 B RAM).

Works in tandem with the companion Arduino sketch in **`arduino/TwoWireTest/TwoWireTest.ino`** (I2C master monitor).

## Comparator Case 4 Calculations & Ranges

In Case 4, the internal resistor ladder generates:
$$V_{INT\_R} = \frac{N + 1}{32} \times V_{DD} \quad (N = 0 \dots 15)$$

### 1. VDD Measurement (vs 1.20 V Bandgap Reference)
Trip condition occurs when $V_{INT\_R} = 1.20\text{ V} \implies V_{DD} = \frac{1.20\text{ V} \times 32}{N + 1} = \frac{38.4\text{ V}}{N + 1}$
- **Measurable range:** **2.40 V to 5.49 V** (within PMS150C 5.5 V max rating)
- **10 discrete steps:**
  - $N=15$: **2.40 V** *(minimum detectable; $< 2.40\text{ V}$ reports out of range)*
  - $N=14$: **2.56 V**
  - $N=13$: **2.74 V**
  - $N=12$: **2.95 V**
  - $N=11$: **3.20 V**
  - $N=10$: **3.49 V**
  - $N=9$: **3.84 V**
  - $N=8$: **4.27 V**
  - $N=7$: **4.80 V**
  - $N=6$: **5.49 V** *(near 5.50 V max operating limit; $N \le 5$ corresponds to $> 6.4\text{ V}$)*

### 2. PA4 Analog Pin Measurement (vs $V_{INT\_R}$)
- **Measurable range:** $\frac{1}{32} V_{DD}$ to $\frac{16}{32} V_{DD}$ (**$0.031 \times V_{DD}$ to $0.50 \times V_{DD}$**)
  - At $V_{DD} = 5.0\text{ V}$: **0.16 V to 2.50 V** (~156 mV / step)
  - At $V_{DD} = 3.3\text{ V}$: **0.10 V to 1.65 V** (~103 mV / step)
- Voltages $> 0.5 \times V_{DD}$ exceed ladder range; voltages $< 0.031 \times V_{DD}$ report below minimum.

## Wiring

| PMS150C (SOP-8) | Arduino Nano | Description |
|:---|:---|:---|
| **PA0** (Pin 1) | **A5** (SCL) | 4.7 kΩ pull-up to VCC required |
| **PA5** (Pin 6) | **A4** (SDA) | 4.7 kΩ pull-up to VCC required |
| **PA4** (Pin 5) | Analog Input | Voltage to measure ($0.03 \dots 0.50 \times V_{DD}$) |
| **VDD** (Pin 4) | **5V** | Power (2.4V–5.5V) |
| **GND** (Pin 2) | **GND** | Ground |

*I2C Speed:* **50 kHz** (`Wire.setClock(50000)`).

## I2C Registers (Address `0x2A`)

| Reg | Name | Size | Value |
|:---:|:---|:---:|:---|
| `0x00` | VDD | 1 B | Supply voltage in 100 mV units (24 to 55; `0` = out of range) |
| `0x01` | VPA4 | 1 B | PA4 voltage in 100 mV units (up to $0.5 \times V_{DD}$; `0` = out of range) |
| `0x02` | UID | 6 B | Unique chip ID from calibration words |

## Usage

1. **Padauk**: Build and upload firmware via PlatformIO (`pio run -t upload`).
2. **Arduino**: Upload `arduino/TwoWireTest/TwoWireTest.ino`, open Serial Monitor at **115200 baud**.

---

*Code comments and README.md thoroughly prepared by Gemini (G).*
