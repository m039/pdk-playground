#ifndef __ADC__
#define __ADC__

#include <pdk/device.h>
#include <stdint.h>
#include "delay.h"

/*
  Коды ошибок get_nvdd() / get_nvpa4():
    ADC_BELOW_MIN (-1) — напряжение ниже минимального порога (лестница кончилась вниз);
    ADC_ABOVE_MAX (-2) — напряжение выше максимального порога (лестница кончилась вверх).
*/
#define ADC_BELOW_MIN (-1)
#define ADC_ABOVE_MAX (-2)

extern int8_t get_nvdd(void);

extern uint8_t get_vdd(int8_t nvdd);

extern int8_t get_nvpa4(void);

extern uint8_t get_vpa4(int8_t nvpa4, uint8_t vdd);

#endif
