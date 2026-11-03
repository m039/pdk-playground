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
  Measurable VDD threshold constants for Comparator Case 4 (in decivolts, 1 unit = 100 mV):
  Switching condition: VINT_R = 1.20V => VDD = 1.20V * 32 / (N + 1) = 38.4V / (N + 1)
  Note: Threshold voltages may vary with a 1-2% tolerance depending on operating clock frequency.
  Use one of these for ADC_VOLTAGE_THRESHOLD:
*/
#define ADC_VOLTAGE_2V40 24 /* N = 15: 2.40 V (minimum detectable VDD) */
#define ADC_VOLTAGE_2V56 25 /* N = 14: 2.56 V */
#define ADC_VOLTAGE_2V74 27 /* N = 13: 2.74 V */
#define ADC_VOLTAGE_2V95 29 /* N = 12: 2.95 V */
#define ADC_VOLTAGE_3V20 32 /* N = 11: 3.20 V */
#define ADC_VOLTAGE_3V49 34 /* N = 10: 3.49 V */
#define ADC_VOLTAGE_3V84 38 /* N =  9: 3.84 V */
#define ADC_VOLTAGE_4V27 42 /* N =  8: 4.27 V */
#define ADC_VOLTAGE_4V80 48 /* N =  7: 4.80 V */
#define ADC_VOLTAGE_5V49 54 /* N =  6: 5.49 V (near 5.50V max operating rating) */

/* Aliases with ADC_VOLTAGE_THRESHOLD_ prefix */
#define ADC_VOLTAGE_THRESHOLD_2V40 ADC_VOLTAGE_2V40
#define ADC_VOLTAGE_THRESHOLD_2V56 ADC_VOLTAGE_2V56
#define ADC_VOLTAGE_THRESHOLD_2V74 ADC_VOLTAGE_2V74
#define ADC_VOLTAGE_THRESHOLD_2V95 ADC_VOLTAGE_2V95
#define ADC_VOLTAGE_THRESHOLD_3V20 ADC_VOLTAGE_3V20
#define ADC_VOLTAGE_THRESHOLD_3V49 ADC_VOLTAGE_3V49
#define ADC_VOLTAGE_THRESHOLD_3V84 ADC_VOLTAGE_3V84
#define ADC_VOLTAGE_THRESHOLD_4V27 ADC_VOLTAGE_4V27
#define ADC_VOLTAGE_THRESHOLD_4V80 ADC_VOLTAGE_4V80
#define ADC_VOLTAGE_THRESHOLD_5V49 ADC_VOLTAGE_5V49


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
