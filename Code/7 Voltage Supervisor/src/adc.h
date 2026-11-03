#ifndef __ADC__
#define __ADC__

#include <pdk/device.h>
#include <stdint.h>
#include "delay.h"

/*
  Error codes returned by get_nvdd() and get_nvpa4():
    ADC_BELOW_MIN (-1) - Voltage below minimum ladder threshold (underflow).
    ADC_ABOVE_MAX (-2) - Voltage above maximum ladder threshold (overflow).
*/
#define ADC_BELOW_MIN (-1)
#define ADC_ABOVE_MAX (-2)

/*
  Measures VDD using the internal comparator with 1.2V bandgap reference (Case 4).
  Thresholds: VDD = 38.4V / (N + 1).
  Measurable range: 2.40 V (N=15) to 5.49 V (N=6). Values < 2.40V return ADC_BELOW_MIN.
  Returns ladder step index (0-15) or an error code (< 0).
*/
extern int8_t get_nvdd(void);

/*
  Converts VDD ladder step index to voltage in decivolts (1 unit = 100 mV).
  Returns 0 if out of range.
*/
extern uint8_t get_vdd(int8_t nvdd);

/*
  Measures analog voltage on PA4 using the internal resistor ladder (Case 4).
  Measurable range: (1/32)*VDD to (16/32)*VDD (0.031*VDD to 0.50*VDD).
    - At VDD = 5.0V: ~0.16 V to 2.50 V (~156 mV/step)
    - At VDD = 3.3V: ~0.10 V to 1.65 V (~103 mV/step)
  Returns ladder step index (0-15) or an error code (< 0).
*/
extern int8_t get_nvpa4(void);

/*
  Calculates PA4 voltage in decivolts (1 unit = 100 mV) given PA4 and VDD step indices:
  VPA4 = (nvpa4 + 1) / 32 * VDD.
*/
extern uint8_t get_vpa4(int8_t nvpa4, int8_t nvdd);

#endif
