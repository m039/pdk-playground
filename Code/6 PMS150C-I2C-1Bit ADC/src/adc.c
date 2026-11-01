#include "adc.h"


static uint8_t i;

/* Case 4: ступень N лестницы = (N+1)/32 * VDD; VDD = 120*32/(N+1) децимилливольт. */
#define VOLTAGE(n) (uint8_t)((12. * 32. / (n + 1.)) > 255 ? 0xff: (12. * 32. / (n + 1.)))

/*
  Измерение VDD: сколько ступеней лестницы (Case 4) ещё выше bandgap 1.2 В.

  PLUS  = VINT_R = (N+1)/32 * VDD  — порог РАСТЁТ с ростом N;
  MINUS = bandgap 1.2 В.
  positive = "VINT_R выше 1.2 В". Перебираем N снизу вверх и берём ПЕРВОЕ
  positive: это ступень, на которой порог ещё не провалился под 1.2 В.

  Результат: nvdd = N означает VDD в интервале
      ( 1200*32/(N+2) , 1200*32/(N+1) ]  мВ
  т.е. возвращается НИЖНЯЯ граница интервала (с точностью до ступени).

  ADC_BELOW_MIN (-1): positive нет ни на одной ступени → VDD <= ADC_VDD_MIN (2.40 В).
  ADC_ABOVE_MAX (-2): positive уже на ступени 0 → VDD/32 > 1.2 В, т.е.
      VDD > ADC_VDD_MAX (38.4 В) — за пределами лестницы (на практике недостижимо).
*/
int8_t get_nvdd(void)
{
    // Внутренние входы: ножки не участвуют, утечки не влияют.
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);

    for (i = 0; i < 16; i++)
    {
        // GPCS пишем ЦЕЛИКОМ: биты case [5:4] и уровня [3:0] write-only,
        // read-modify-write затёр бы выбор case нулями.
        GPCS = (1 << 5) | (1 << 4) | i; // Case 4
        _delay_us(10);                  // установление компаратора после смены порога

        if (GPCC & GPCC_COMP_RESULT_POSITIVE)
        {
            if (i == 0)
            {
                return ADC_ABOVE_MAX; // порог выше лестницы: VDD > ADC_VDD_MAX
            }
            return i;
        }
    }
    return ADC_BELOW_MIN; // лестница кончилась: VDD <= ADC_VDD_MIN
}

/*
  Измерение напряжения на PA4 той же лестницей (Case 4).

  PLUS  = PA4 (измеряемый сигнал через внешний делитель);
  MINUS = VINT_R = (N+1)/32 * VDD — порог РАСТЁТ с ростом N.
  positive = "PA4 выше порога". Первый НЕ-positive = первая ступень,
  где порог уже догнал PA4: это ВЕРХНЯЯ граница интервала.

  Результат: nvpa4 = N означает PA4 в интервале
      ( N/32 * VDD , (N+1)/32 * VDD ]  (доли от VDD)

  ADC_BELOW_MIN (-1): порог догнал PA4 уже на ступени 0 → PA4 <= 1/32 * VDD,
      т.е. ниже ADC_PA4_MIN_NUM/ADC_PA4_DEN * VDD.
  ADC_ABOVE_MAX (-2): PA4 выше порога на всех ступенях → PA4 > 15/32 * VDD,
      т.е. выше ADC_PA4_MAX_NUM/ADC_PA4_DEN * VDD.
*/
int8_t get_nvpa4(void)
{
    GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_VINT_R | GPCC_COMP_PLUS_PA4);

    for (i = 0; i < 16; i++)
    {
        GPCS = (1 << 5) | (1 << 4) | i; // Case 4
        _delay_us(10);                  // установление компаратора после смены порога

        if (!(GPCC & GPCC_COMP_RESULT_POSITIVE))
        {
            if (i == 0)
            {
                return ADC_BELOW_MIN; // PA4 ниже нижней ступени: < 1/32 * VDD
            }
            return i;
        }
    }
    return ADC_ABOVE_MAX; // PA4 выше верхней ступени: > 15/32 * VDD
}

/*
  Пересчёт индекса ступени в VDD. Единицы: децимилливольты (1 ед. = 10 мВ),
  чтобы обойтись без float:  VDD[мВ] = 1200*32/(N+1) = 120*32/(N+1) * 10.

  Возвращает НИЖНЮЮ границу интервала (реальный VDD чуть выше напечатанного,
  до следующей ступени: при N=15 это 240 ед. = 2.40 В, при N=8 — 426 ед. = 4.27 В).

  0 — маркер "вне шкалы" (nvdd = ADC_BELOW_MIN или ADC_ABOVE_MAX),
  не путать с нулевым напряжением.
*/
uint8_t get_vdd(int8_t nvdd)
{
    if (nvdd == ADC_BELOW_MIN)
    {
        nvdd = 0;;
    }
    else if (nvdd == ADC_ABOVE_MAX)
    {
        nvdd = 15;
    }
    else if (nvdd < 0)
    {
        return 0;
    }

    static const uint8_t voltages[] = {
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
    return voltages[nvdd];
}

/*
  Пересчёт PA4 в децимилливольты.

  PA4 = (nvpa4+1)/32 * VDD. Деление на 32 — сдвиг вправо на 5 (ADC_PA4_DEN).
  Формулы Case 4 для VDD и PA4 сокращают делитель: деления не нужно вообще.

  Переполнение uint16_t не грозит: (nvpa4+1) <= ADC_PA4_MAX_NUM+1 = 16,
  VDD <= 640 ед., произведение <= 10240 — влезает с запасом.

  Возвращает ВЕРХНЮЮ границу интервала (реальный PA4 чуть ниже напечатанного).
  0 — маркер "вне шкалы" (любой из кодов ошибок в аргументах).
*/
uint8_t get_vpa4(int8_t nvpa4, int8_t nvdd)
{
    if (nvdd == ADC_BELOW_MIN)
    {
        nvdd = 0;;
    }
    else if (nvdd == ADC_ABOVE_MAX)
    {
        nvdd = 15;
    }
    else if (nvdd < 0)
    {
        return 0;
    }

    if (nvpa4 == ADC_BELOW_MIN)
    {
        nvpa4 = 0;;
    }
    else if (nvpa4 == ADC_ABOVE_MAX)
    {
        nvpa4 = 15;
    }
    else if (nvpa4 < 0)
    {
        return 0;
    }

    uint8_t vdd = get_vdd(nvdd);

    // Вместо "тяжелого" умножения uint16_t делаем быстрый цикл сложений.
    // Так как (nvpa4 + 1) максимум равен 16, этот цикл займет минимум места в ROM.
    uint16_t res = 0;
    i = nvpa4 + 1;
    do {
        res += vdd;
    } while (--i);

    return (uint8_t)(res >> 5); // Сдвиг на 32 компилятор делает очень эффективно
}
