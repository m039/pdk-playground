# PMS150C I2C UID Chip & Arduino Reader

Emulates an ultra-low-cost I2C hardware identification / dongle chip on a Padauk **PMS150C** (or **PFS154**). The microcontroller acts as an I2C slave (7-bit address `0x2B`) and responds with a 6-byte Unique ID (UID) sequence from Register `0x00`.

To achieve true sub-microamp standby consumption, the firmware automatically enters the ultra-low-power **`stopsys`** deep sleep mode after 65536 idle cycles.

Works in tandem with the companion Arduino sketch in **`arduino/TwoWireTest/TwoWireTest.ino`** (I2C master reader).

---

## Power Consumption Estimation

The microcontroller operates at **8 MHz** internal high-speed oscillator (IHRC) without any analog peripherals (comparator, ADC, and Bandgap reference are disabled):

| State | Clock / Mode | Current at 5.0V | Current at 3.3V | Description |
|:---|:---|:---:|:---:|:---|
| **Active Mode** | IHRC 8 MHz | **~2.0 – 2.5 mA** | **~1.2 – 1.5 mA** | CPU actively polling I2C bus and servicing communication |
| **Deep Sleep Mode (`stopsys`)** | All clocks stopped (IHRC off) | **~0.5 – 1.0 µA** | **< 0.5 µA** | Entered after 65536 idle passes; clock switched to ILRC and IHRC disabled |

> **Bus Traffic & Power Note:**
> Because the microcontroller wakes up on any signal toggle on SCL (`PA0`) or SDA (`PA5`) via `PADIER` pin-change detection, any communication on a shared I2C bus—even if addressed to other devices—will repeatedly wake the chip or reset its idle timeout. As long as there is active traffic on the I2C bus, the MCU will remain awake and consume full active power (**~2.0 – 2.5 mA** at 5.0V / **~1.2 – 1.5 mA** at 3.3V).

---

## Wiring (PMS150C to Arduino Nano)

Both I2C lines are open-drain and require external 4.7 kΩ pull-up resistors to VCC.

| PMS150C (SOP-8) | Arduino Nano | Description | Notes |
|:---|:---|:---|:---|
| **Pin 1 (PA0)** | **A5** (SCL) | I2C Clock line | 4.7 kΩ pull-up to 5V required |
| **Pin 6 (PA5)** | **A4** (SDA) | I2C Data line | 4.7 kΩ pull-up to 5V required |
| **Pin 4 (VDD)** | **5V** (or 3V3) | Power supply | 2.4V – 5.5V DC |
| **Pin 2 (GND)** | **GND** | Ground | Common ground |

---

## Wake-Up & Communication Protocol

Because the PMS150C enters `stopsys` deep sleep when idle:

1. **Wake-Up Ping:** The master transmits a dummy address packet (`Wire.beginTransmission(0x2B); Wire.endTransmission(true);`).
2. **Wake Latency:** The master waits **1 ms** (`delay(1)`, range 1–125 ms works) allowing the PMS150C 8 MHz IHRC oscillator to start up, stabilize, and enter `ST_IDLE`.
3. **Register Read:** The master transmits the requested register index (`0x00`) and requests 6 bytes (`Wire.requestFrom(0x2B, 6)`).
4. **Data:** The slave streams the 6-byte UID sequence (`ID[0..5]`).
5. **Idle Timeout:** If no bus activity occurs for 65536 loop cycles, the slave switches clock to ILRC, turns off IHRC, and re-enters `stopsys`.

---

## I2C Registers (Address `0x2B`)

| Register | Name | Length | Description |
|:---:|:---|:---:|:---|
| `0x00` | `UID` | 6 Bytes | 6-byte Unique ID payload (e.g. `22:55:33:11:33:53`) |

---

## Usage

1. **Padauk Firmware:**
   - Open [`platformio.ini`](platformio.ini) in VS Code with PlatformIO.
   - Build and upload firmware to the PMS150C using the Easy PDK Programmer:
     ```bash
     pio run -t upload
     ```
2. **Arduino Master:**
   - Open `arduino/TwoWireTest/TwoWireTest.ino` in the Arduino IDE.
   - Connect your Arduino Nano via USB and select the corresponding COM port.
   - Upload sketch and open Serial Monitor at **115200 baud**.

---

*Code comments and README.md thoroughly prepared by Gemini (G).*
