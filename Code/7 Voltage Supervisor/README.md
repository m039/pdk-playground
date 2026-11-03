# Voltage Supervisor

A supply voltage supervisor (brownout detector / power-good monitor) implemented on Padauk microcontrollers (PFS154, PMS150C) using the internal analog comparator with the internal resistor ladder and 1.20V bandgap reference (**Comparator Case 4**).

The output pin **PA4** indicates voltage status:
- **HIGH (`1`)**: Supply voltage $V_{DD} \ge V_{threshold}$.
- **LOW (`0`)**: Supply voltage $V_{DD} < V_{threshold} - V_{hysteresis}$.

The comparator dynamically updates its ladder tap between `nvdd` and `nvdd - 1` to create clean switching with hardware hysteresis and prevent oscillations near the boundary.

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

*(Values $N \le 5$ correspond to $V_{DD} > 6.4\text{ V}$, which exceeds the maximum operating voltage of the MCU).*

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

> **Compile-time validation:** If `ADC_VOLTAGE_THRESHOLD` is not specified or an invalid value is supplied, compilation will immediately fail with a descriptive `#error` directive explaining valid options.
