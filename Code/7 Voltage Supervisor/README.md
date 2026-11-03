# Voltage Supervisor

A supply voltage supervisor (brownout detector / power-good monitor) implemented on Padauk microcontrollers (PFS154, PMS150C) using the internal analog comparator with the internal resistor ladder and 1.20V bandgap reference (**Comparator Case 4**).

> **Hardware note (Power Consumption):** For PMS150C, the analog comparator does not function in sleep mode (`stopsys`) and cannot wake up the core. Therefore, to continuously monitor voltage, the MCU remains active in slow mode: the internal high-speed oscillator (IHRC) is disabled and the system clock is switched to the internal low-speed oscillator (ILRC) with maximum divider to reduce frequency to a minimum. This keeps total active current consumption at **~50 µA maximum** (MCU core running on ILRC ~15–20 µA + comparator, 1.2V bandgap reference, and internal resistor ladder ~25–30 µA).

The output pin **PA4** indicates voltage status:
- **LOW (`0`)**: Supply voltage $V_{DD} < V_{threshold}$.
- **HIGH (`1`)**: Supply voltage $V_{DD} \ge V_{threshold} + V_{hysteresis}$.

The comparator dynamically updates its ladder tap between `nvdd` and `nvdd - 1` to create clean switching with software hysteresis and prevent oscillations near the boundary.

---

## Comparator Case 4 Thresholds

The internal resistor ladder switches at:
$$V_{INT\_R} = \frac{N + 1}{32} \times V_{DD} = 1.20\text{ V} \implies V_{DD} = \frac{38.4\text{ V}}{N + 1}$$

| Macro Name | Value (decivolts) | Ladder Step ($N$) | Nominal Threshold ($V_{DD}$) |
|:---|:---:|:---:|:---:|
| `ADC_VOLTAGE_2V40` | `24` | $N = 15$ | 2.40 V (minimum detectable) |
| `ADC_VOLTAGE_2V56` | `25` | $N = 14$ | 2.56 V |
| `ADC_VOLTAGE_2V74` | `27` | $N = 13$ | 2.74 V |
| `ADC_VOLTAGE_2V95` | `29` | $N = 12$ | 2.95 V |
| `ADC_VOLTAGE_3V20` | `32` | $N = 11$ | 3.20 V |
| `ADC_VOLTAGE_3V49` | `34` | $N = 10$ | 3.49 V |
| `ADC_VOLTAGE_3V84` | `38` | $N = 9$  | 3.84 V |
| `ADC_VOLTAGE_4V27` | `42` | $N = 8$  | 4.27 V |
| `ADC_VOLTAGE_4V80` | `48` | $N = 7$  | 4.80 V |
| `ADC_VOLTAGE_5V49` | `54` | $N = 6$  | 5.49 V (near 5.50V max rating) |

> *Note:* Threshold voltages may vary by approximately 1–2% depending on the operating frequency of the microcontroller.

---

## Configuration

The threshold can be selected in two ways:

### 1. In `platformio.ini` (Recommended)

Set `build_flags` in `platformio.ini` using either the macro name or the numeric decivolt value:

```ini
build_flags =
    -DADC_VOLTAGE_THRESHOLD=ADC_VOLTAGE_3V20
    ; or: -DADC_VOLTAGE_THRESHOLD=32
```

### 2. In `src/main.c`

Uncomment and set `ADC_VOLTAGE_THRESHOLD`:

```c
#define ADC_VOLTAGE_THRESHOLD ADC_VOLTAGE_3V20
```

---

*Code comments and README.md thoroughly prepared by Gemini (G).*
