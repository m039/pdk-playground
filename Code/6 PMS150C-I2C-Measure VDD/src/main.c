/*
  Измеритель напряжения PMS150C через встроенный компаратор (1-bit ADC).

  Питание чипа (VDD) и напряжение на PA4 измеряются перебором ступеней
  внутренней резисторной лестницы VINT_R (Case 4: VINT_R = (N+1)/32 * VDD)
  относительно опорного bandgap 1.2 В.

  Результат печатается в UART (софт-сериал из easypdk, бит-бэнг через TM2
  прерывание) в ДЕЦИМИЛЛИВОЛЬТАХ: 1 единица = 10 мВ. Целочисленная арифметика
  без деления и float — всё сводится к сдвигам и таблице констант.
*/

#define DEBUG 1

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

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

/*
  Измерение VDD: сколько ступеней лестницы (Case 4) ещё выше bandgap 1.2 В.

  PLUS  = VINT_R = (N+1)/32 * VDD  — порог РАСТЁТ с ростом N;
  MINUS = bandgap 1.2 В.
  positive = "VINT_R выше 1.2 В". Перебираем N снизу вверх и берём ПЕРВОЕ
  positive: это ступень, на которой порог ещё не провалился под 1.2 В.

  Результат: nvdd = N означает VDD в интервале
      ( 1200*32/(N+2) , 1200*32/(N+1) ]  мВ
  т.е. возвращается НИЖНЯЯ граница интервала (с точностью до ступени).
  -1 = лестница кончилась, VDD ниже 2.4 В (все ступени ниже bandgap).
*/
int8_t get_nvdd()
{
  // Внутренние входы: ножки не участвуют, утечки не влияют.
  GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);

  for (int8_t i = 0; i < 16; i++)
  {
    // GPCS пишем ЦЕЛИКОМ: биты case [5:4] и уровня [3:0] write-only,
    // read-modify-write затёр бы выбор case нулями.
    GPCS = (1 << 5) | (1 << 4) | i; // Case 4
    _delay_us(10);                  // установление компаратора после смены порога

    if (GPCC & GPCC_COMP_RESULT_POSITIVE)
    {
      return i;
    }
  }
  return -1;
}

/*
  Измерение напряжения на PA4 той же лестницей (Case 4).

  PLUS  = PA4 (измеряемый сигнал через внешний делитель);
  MINUS = VINT_R = (N+1)/32 * VDD — порог РАСТЁТ с ростом N.
  positive = "PA4 выше порога". Первый НЕ-positive = первая ступень,
  где порог уже догнал PA4: это ВЕРХНЯЯ граница интервала.

  Результат: nvpa4 = N означает PA4 в интервале
      ( N/32 * VDD , (N+1)/32 * VDD ]  (доли от VDD)
  -1 = PA4 выше верхней ступени (15/32 * VDD), лестница не дотянулась.
*/
int8_t get_nvpa4()
{
  GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_VINT_R | GPCC_COMP_PLUS_PA4);

  for (int8_t i = 0; i < 16; i++)
  {
    GPCS = (1 << 5) | (1 << 4) | i; // Case 4
    _delay_us(10);                  // установление компаратора

    if (!(GPCC & GPCC_COMP_RESULT_POSITIVE))
    {
      return i;
    }
  }
  return -1;
}

/*
  Пересчёт индекса ступени в VDD. Единицы: децимилливольты (1 ед. = 10 мВ),
  чтобы обойтись без float:  VDD[мВ] = 1200*32/(N+1) = 120*32/(N+1) * 10.

  Возвращает НИЖНЮЮ границу интервала (реальный VDD чуть выше напечатанного,
  до следующей ступени: при N=15 это 240 ед. = 2.40 В, при N=8 — 426 ед. = 4.27 В).

  0 — маркер "вне шкалы" (nvdd < 0: VDD ниже 2.4 В), не путать с нулевым напряжением.
*/
uint16_t get_vdd(int8_t nvdd)
{
  if (nvdd < 0)
  {
    return 0; // ниже диапазона (VDD < 2.4 В) — маркер «вне шкалы»
  }
#define VOLTAGE(n) (120 * 32 / (n + 1)) // Case 4, децимилливольты
  static const uint16_t voltages[] = {
      VOLTAGE(0),
      VOLTAGE(1),
      VOLTAGE(2),
      VOLTAGE(3),
      VOLTAGE(4),
      VOLTAGE(5),
      VOLTAGE(6),
      VOLTAGE(7),
      VOLTAGE(8),
      VOLTAGE(9),
      VOLTAGE(10),
      VOLTAGE(11),
      VOLTAGE(12),
      VOLTAGE(13),
      VOLTAGE(14),
      VOLTAGE(15),
  };
#undef VOLTAGE
  return voltages[nvdd];
}

/*
  Пересчёт PA4 в децимилливольты.

  PA4 = (nvpa4+1)/32 * VDD. Деление на 32 — сдвиг вправо на 5.
  Формулы Case 4 для VDD и PA4 сокращают делитель: деления не нужно вообще.

  Переполнение uint16_t не грозит: (nvpa4+1) <= 16, VDD <= 640 ед.,
  произведение <= 10240 — влезает с запасом.

  Возвращает ВЕРХНЮЮ границу интервала (реальный PA4 чуть ниже напечатанного).
  0 — маркер "вне шкалы" (nvpa4 < 0: сигнал выше 15/32 * VDD, либо ошибка VDD).
*/
uint16_t get_vpa4(int8_t nvpa4, int8_t nvdd)
{
  if (nvpa4 < 0 || nvdd < 0)
  {
    return 0; // вне диапазона
  }

  return (uint16_t)((uint8_t)(nvpa4 + 1) * get_vdd(nvdd)) >> 5;
}

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
    int8_t nvpa4 = get_nvpa4();

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
