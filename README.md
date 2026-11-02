# Various Examples with Padauk MCU

A collection of tested, ready-to-flash firmware examples and hardware experiments for ultra-low-cost Padauk 8-bit microcontrollers (PFS154, PMS150C, PFS122, PMS150G), built with [pdk-platformio](https://github.com/m039/pdk-platformio) and the SDCC toolchain.

---

## Projects Overview

| # | Project | Target MCU | Description |
|:---:|:---|:---|:---|
| 1 | [Blink](Code/1%20Blink) | PFS154 / All | Basic LED blinking in C (1 s interval) |
| 2 | [BlinkAsm](Code/2%20BlinkAsm) | PFS154 | LED blinking written directly in pure Padauk assembly (`.asm`) |
| 3 | [Led PWM](Code/3%20Led%20PWM) | PFS154 / PMS150C | Smooth LED breathing / fading via hardware 8-bit PWM |
| 4 | [Musical Melody (Period Mode)](Code/4%20Musical%20Melody%20(Period%20Mode)) | PFS154, PMS150C, PMS150G, PFS122 | Melody player on a piezo buzzer with button wake-up from `stopsys` deep sleep |
| 5 | [Musical Melody (PWM Mode)](Code/5%20Musical%20Melody%20(PWM%20Mode)) | PFS122 | Tone generation with real-time volume adjustment via analog potentiometer |
| 6 | [PMS150C-I2C-1Bit ADC](Code/6%20PMS150C-I2C-1Bit%20ADC) | PMS150C (tested on PFS154) | 1-bit comparator ADC voltage meter (VDD & PA4) + I2C slave with companion Arduino master |

---

## Getting Started

1. **Prerequisites:**
   - **VS Code** with the **[PlatformIO IDE](https://platformio.org/install/ide?install=vscode)** extension.
   - An **[Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software)** (or compatible programmer) connected via USB.
2. **Open Workspace:**
   - Open [`Code/pdk-playground.code-workspace`](Code/pdk-playground.code-workspace) in VS Code to load all example projects into your workspace.
3. **Build & Upload:**
   - Select the desired project environment in the PlatformIO project switcher (bottom status bar).
   - Click **Build** (`✓`) or **Upload** (`→`) on the PlatformIO toolbar. PlatformIO will automatically fetch the [`pdk-platformio`](https://github.com/m039/pdk-platformio) platform package on first run.

## References

- [pdk-platformio](https://github.com/m039/pdk-platformio) - PlatformIO platform for Padauk microcontrollers
- [Free-PDK](https://github.com/free-pdk) - Open-source tools, includes, and documentation for Padauk MCUs
- [Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software) - In-circuit programmer and clock calibrator
