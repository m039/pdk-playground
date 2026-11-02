# Various Examples with Padauk MCU

A collection of tested, ready-to-flash firmware examples and hardware experiments for ultra-low-cost Padauk 8-bit microcontrollers (PFS154, PMS150C, PFS122, PMS150G), built with [pdk-platformio](https://github.com/m039/pdk-platformio) and the SDCC toolchain.

---

## Projects Overview

| # | Project | Target MCU | Key Peripherals | Description |
|:---:|:---|:---|:---|:---|
| 1 | [Blink](Code/1%20Blink) | PFS154 / All | GPIO, Delay loops | Basic LED blinking in C (1 s interval) |
| 2 | [BlinkAsm](Code/2%20BlinkAsm) | PFS154 / pdk14 | Assembly, GPIO | LED blinking written directly in pure Padauk assembly (`.asm`) |
| 3 | [Led PWM](Code/3%20Led%20PWM) | PFS154 / PMS150C | Timer 2 (PWM) | Smooth LED breathing / fading via hardware 8-bit PWM |
| 4 | [Musical Melody (Period Mode)](Code/4%20Musical%20Melody%20(Period%20Mode)) | PFS154, PMS150C, PMS150G, PFS122 | Timer 2 (Period), Sleep | Melody player on a piezo buzzer with button wake-up from `stopsys` deep sleep |
| 5 | [Musical Melody (PWM Mode)](Code/5%20Musical%20Melody%20(PWM%20Mode)) | PFS122 | Timer 2 (PWM), 8-bit ADC | Tone generation with real-time volume adjustment via analog potentiometer |
| 6 | [PMS150C-I2C-1Bit ADC](Code/6%20PMS150C-I2C-1Bit%20ADC) | PMS150C (tested on PFS154) | Comparator (Case 4), Software I2C | 1-bit comparator ADC voltage meter (VDD & PA4) + I2C slave with companion Arduino master |

---

## Getting Started

### Prerequisites

1. **[PlatformIO](https://platformio.org/)** (VSCode extension or CLI `pio`).
2. **[pdk-platformio](https://github.com/m039/pdk-platformio)** platform integration (automatically fetched by PlatformIO).
3. **Hardware Programmer:** [Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software) (or compatible hardware) to program and calibrate Padauk ICs over the serial interface.

### Building & Uploading

Navigate to any project directory and run:

```bash
# Build firmware
pio run

# Build and flash to connected Padauk MCU
pio run -t upload
```

For projects supporting multiple MCU targets (e.g. Project 4):
```bash
# Flash specific environment
pio run -e pfs154 -t upload
pio run -e pms150c -t upload
```

---

## Flash vs. OTP Microcontrollers

- **Flash MCUs** (e.g. **PFS154**, **PFS122**): Support multiple erase/re-flash cycles. Recommended for prototyping, experimenting, and debugging firmware.
- **OTP MCUs** (e.g. **PMS150C**, **PMS150G**): One-Time Programmable chips costing only a few cents. Once verified on flash devices, firmware can be flashed to OTP chips for production.

---

## References

- [pdk-platformio](https://github.com/m039/pdk-platformio) - PlatformIO platform for Padauk microcontrollers
- [Free-PDK](https://github.com/free-pdk) - Open-source tools, includes, and documentation for Padauk MCUs
- [Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software) - In-circuit programmer and clock calibrator
