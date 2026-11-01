/*
  Измеритель напряжения PMS150C через встроенный компаратор (1-bit ADC).

  Питание чипа (VDD) и напряжение на PA4 измеряются перебором ступеней
  внутренней резисторной лестницы VINT_R (Case 4: VINT_R = (N+1)/32 * VDD)
  относительно опорного bandgap 1.2 В.

  Результат печатается в UART (софт-сериал из easypdk, бит-бэнг через TM2
  прерывание) в ДЕЦИМИЛЛИВОЛЬТАХ: 1 единица = 10 мВ. Целочисленная арифметика
  без деления и float — всё сводится к сдвигам и таблице констант.
*/

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"
#include "adc.h"

#if defined(PFS154)
#define DEBUG 1
#else
#define DEBUG 0
#endif

#if DEBUG != 0
#include "serial.h"
#include <stdlib.h>
#endif

/*
  Отладочный вывод через прерывание TM2: serial_irq_handler() выставляет
  каждый бит TX по тику таймера, поэтому мейн-цикл не блокируется на печать.
*/
#if DEBUG != 0
void interrupt(void) __interrupt(0)
{
  if (INTRQ & INTRQ_TM2)
  {
    INTRQ &= ~INTRQ_TM2;        // сбросить флаг, иначе ISR зациклится
    serial_irq_handler();       // отправить следующий бит TX
  }
}

/* Печать префикса и числа (в децимилливольтах) с переводом строки. */
void print_number(const char *prefix, uint16_t number)
{
  const uint8_t *p = prefix;
  while (*p != 0)
  {
    putchar(*p);
    p++;
  }
  char buffer[6];               // 5 цифр (макс. 65535) + нуль-терминатор
  __itoa(number, buffer, 10);
  puts(buffer);
}
#endif

void main()
{

#if DEBUG != 0
  serial_setup();
  INTRQ = 0;      // погасить случайные флаги до включения прерываний
  __engint();     // разрешить прерывания (для TM2/serial)
#endif

  while (1)
  {

#if DEBUG != 0
    // Измеряем один раз и переиспользуем: get_vdd() константный,
    // но повторный вызов get_nvdd() дал бы лишние 16 итераций компаратора.
    int8_t nvdd = get_nvdd();
    uint16_t vdd = get_vdd(nvdd);

    // Окно 4.00–6.00 В в децимилливольтах.
    if (vdd >= 400 && vdd <= 600)
    {
      serial_println("VDD in range");
    }
    else
    {
      serial_println("VDD out of range");
    }

    print_number("VDD: ", vdd);

    int8_t nvpa4 = get_nvpa4();

    print_number("1-bit ADC on PA4: ", get_vpa4(nvpa4, nvdd));
#endif

    _delay_ms(1000);
  }
}

/*
  Startup code - Setup/calibrate system clock.
  Выполняется до инициализации данных; return 0 — продолжить нормальную инициализацию.
*/
unsigned char _sdcc_external_startup(void)
{
  // Инициализация CLKMD: выбор IHRC/ILRC/EOSC и делителя по F_CPU из Makefile.
  AUTO_INIT_SYSCLOCK();

  // Заглушка для EasyPdkProg: калибровка внутреннего осциллятора под TARGET_VDD_MV.
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  return 0;
}
