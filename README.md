# Various Examples with Padauk MCU

A collection of examples and hardware experiments for ultra-low-cost Padauk 8-bit microcontrollers (PFS154, PMS150C, PFS122, PMS150G), built with [pdk-platformio](https://github.com/m039/pdk-platformio) and the SDCC toolchain.

---

## Projects Overview

> The first 5 examples are covered in the article: [m039.ru/post/pdk-melodies/](https://m039.ru/post/pdk-melodies/).

| # | Project | Target MCU | Description |
|:---:|:---|:---|:---|
| 1 | [Blink](Code/1%20Blink) | PFS154 / All | Basic LED blinking in C (1 s interval) |
| 2 | [BlinkAsm](Code/2%20BlinkAsm) | PFS154 | LED blinking written directly in pure Padauk assembly (`.asm`) |
| 3 | [Led PWM](Code/3%20Led%20PWM) | PFS154 / PMS150C | Smooth LED breathing / fading via hardware 8-bit PWM |
| 4 | [Musical Melody (Period Mode)](Code/4%20Musical%20Melody%20(Period%20Mode)) | PFS154, PMS150C, PMS150G, PFS122 | Melody player on a piezo buzzer with button wake-up from `stopsys` deep sleep |
| 5 | [Musical Melody (PWM Mode)](Code/5%20Musical%20Melody%20(PWM%20Mode)) | PFS122 | Tone generation with real-time volume adjustment via analog potentiometer |
| 6 | [PMS150C-I2C-1Bit ADC](Code/6%20PMS150C-I2C-1Bit%20ADC) | PMS150C (tested on PFS154) | 1-bit comparator ADC voltage meter (VDD & PA4) + I2C slave with companion Arduino master |
| 7 | [Voltage Supervisor](Code/7%20Voltage%20Supervisor) | PFS154 / PMS150C | Supply voltage supervisor with configurable threshold and hysteresis using internal comparator |
| 8 | [UID Chip](Code/8%20UID%20Chip) | PMS150C / PFS154 | Emulated I2C slave chip returning a 6-byte Unique ID (UID) with ultra-low-power deep sleep (`stopsys`) |

---

## Getting Started

1. **Prerequisites:**
   - **VS Code** with the **[PlatformIO IDE](https://platformio.org/install/ide?install=vscode)** extension.
   - An **[Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software)** (with development firmware) connected via USB.
2. **Open Workspace:**
   - Open [`Code/pdk-playground.code-workspace`](Code/pdk-playground.code-workspace) in VS Code to load all example projects into your workspace.
3. **Build & Upload:**
   - Select the desired project environment in the PlatformIO project switcher (bottom status bar).
   - Click **Build** (`✓`) or **Upload** (`→`) on the PlatformIO toolbar. PlatformIO will automatically fetch the [`pdk-platformio`](https://github.com/m039/pdk-platformio) platform package on first run.

---

## Hardware & Board Connections

Schematics and PCB layouts: [`PCB/Padauk.eprj2`](PCB/Padauk.eprj2) and [`PCB/Images/`](PCB/Images).

### Easy PDK Programmer (`H1 PROG` on `PDK Devboard V1`)

| `H1` Pin | Signal | PMS150C / PMS150G (SOT23-6) | PFS154 (SOP-16) | PFS122 (SOP-16) |
|:---:|:---|:---:|:---:|:---:|
| **1** | **VCC / VDD** | Pin 5 | Pin 5 | Pin 1 |
| **2** | **PA7** | — | Pin 6 | Pin 2 |
| **3** | **PA6** | Pin 3 | Pin 7 | Pin 3 |
| **4** | **PA5** | Pin 4 | Pin 8 | Pin 4 |
| **5** | **PA3** | Pin 6 | Pin 9 | Pin 13 |
| **6** | **PA4** | Pin 1 | Pin 10 | Pin 14 |
| **7** | **PA0** | — | Pin 11 | Pin 15 |
| **8** | **GND** | Pin 2 | Pin 12 | Pin 16 |

### Project Boards (`PCB/`)

- **`PDK Devboard V1`** ([schematic](PCB/Images/SCH_PDK%20Devboard%20V1_1-P1_2025-12-26.png)): `PMS150C` + `PFS154`, `LED2` (`PA4`), `SW2` (`PA3`), `H1` (`PROG`), `H2` (`POWER`).
- **`PDK Sound Player`** ([schematic](PCB/Images/sound-player-schematic.png)): `PMS150C`, `SW1` (`PA4`), `BUZZER1` (`PA3`).
- **Adapters** (`PCB/Padauk.eprj2`): `pms150c-u06-adapter`, `pfs154-s16-adapter`, `pfs122-s16-adapter`, `Dip8 Connector`.

---

## References

- [m039.ru/post/pdk-melodies/](https://m039.ru/post/pdk-melodies/) - Article covering the first 5 examples
- [pdk-platformio](https://github.com/m039/pdk-platformio) - PlatformIO platform for Padauk microcontrollers
- [Free-PDK](https://github.com/free-pdk) - Open-source tools, includes, and documentation for Padauk MCUs
- [Easy PDK Programmer](https://github.com/free-pdk/easy-pdk-programmer-software) - In-circuit programmer and clock calibrator
