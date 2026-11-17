# Blink LED (Assembly Language)

Demonstrates LED blinking on pin PA4 written directly in pure **Padauk Assembly** (`.asm`), using a 32-bit decrement timing loop (`_delay_loop_32`) and direct register bit manipulation (`set1.io` / `set0.io`).

> **Hardware note:** Configured for **PFS154** (pdk14 architecture), easily adaptable to other Padauk MCUs supported by SDCC assembler (`sdasz80`/`sdas`).

## How It Works

- Configures pin PA4 as an output (`set1.io __pac, #4`).
- Toggles PA4 state between HIGH (`set1.io __pa, #4`) and LOW (`set0.io __pa, #4`).
- Implements delays via a 4-byte counter loop calling `_delay_loop_32`.

## Wiring

| Signal | PFS154 (SOP-16) | PMS150C (SOT23-6) | Connection |
|:---|:---:|:---:|:---|
| **PA4** | Pin 10 | Pin 1 | LED with resistor |
| **VDD** | Pin 5 | Pin 5 | 2.0V–5.5V |
| **GND** | Pin 12 | Pin 2 | Ground |

## Usage

Build and upload firmware using PlatformIO:
```bash
pio run -t upload
```

---

*README.md thoroughly prepared by Gemini (G).*
