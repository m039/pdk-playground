#include <pdk/device.h>
#include <stdlib.h>
#include "auto_sysclock.h"
#include "delay.h"
#include "serial.h"

#define CONFIG_PWM_VOLUME 1
#define CONFIG_DEBUG 0

#define NOTE_C4 (uint32_t)(2 << 5 | 15) // 260HZ (262)
#define NOTE_D4 (uint32_t)(2 << 5 | 13) // 300HZ (294)
#define NOTE_E4 (uint32_t)(2 << 5 | 12) // 325Hz (330)
#define NOTE_F4 (uint32_t)(2 << 5 | 11) // 355Hz (349)
#define NOTE_G4 (uint32_t)(2 << 5 | 10) // 390Hz (392)
#define NOTE_A4 (uint32_t)(2 << 5 | 9)  // 434Hz (440)
#define NOTE_B4 (uint32_t)(2 << 5 | 8)  // 488Hz (494)
#define NOTE_C5 (uint32_t)(2 << 5 | 7)  // 558Hz (523)
#define REST (uint32_t)0

#define DURATION_4 (uint32_t)(1000 / 4)
#define DURATION_2 (uint32_t)(1000 / 2)

const uint32_t melody[] = {
    (NOTE_C4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_C4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_G4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_G4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_A4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_A4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_G4 << 24) | (DURATION_2 << 12) | DURATION_2,
    (REST << 24) | (DURATION_4 << 12) | DURATION_4,

    (NOTE_F4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_F4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_E4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_E4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_D4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_D4 << 24) | (DURATION_4 << 12) | DURATION_4,
    (NOTE_C4 << 24) | (DURATION_2 << 12) | DURATION_2,
    (REST << 24) | (DURATION_4 << 12) | DURATION_4
  };

#define PWM_MAX 255

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define ADC_BIT 3 // PB3

#define TONE_FREQ (16000000 / 64)
#define MAX_SCALE (32)

uint8_t adc = 127;

uint8_t lastTone;

char buffer[8] = {0};

void tone(uint8_t frequency);
void pullAdc();

#if CONFIG_DEBUG
extern char *itoa(int value, char *str, int base);

void interrupt(void) __interrupt(0)
{
  if (INTRQ & INTRQ_TM2)
  {                       // TM2 interrupt request?
    INTRQ &= ~INTRQ_TM2;  // Mark TM2 interrupt request processed
    serial_irq_handler(); // Process next Serial Bit
  }
}
#endif

void delay(uint16_t delay)
{
#if CONFIG_PWM_VOLUME
  delay /= 16;

  for (uint16_t i = 0; i < delay; i++)
  {
    pullAdc();
    uint8_t s1 = 0;
    uint8_t s2 = 0;

    for (uint8_t j = 0; j < 0xF; j++)
    {
      if (j < adc && s1 == 0)
      {
        tone(lastTone);
        s1 = 1;
      }
      
      if (j >= adc && s2 == 0)
      {
        tone(0);
        s2 = 1;
      }
      _delay_us(1000);
    }
  }

#else
  for (uint16_t i = 0; i < delay; i++)
  {
    _delay_ms(1);
  }

  pullAdc();
#endif
}

void tone(uint8_t frequency)
{
  if (frequency == 0)
  {
    TM2C = 0;
    TM2S = 0;
    TM2B = 0;
  }
  else
  {
    TM2C = (uint8_t)(TM2C_MODE_PWM | TM2C_OUT_PA3 | TM2C_CLK_IHRC);
    TM2S = frequency;

#if CONFIG_PWM_VOLUME
    TM2B = 127;
#else
    TM2B = adc;
#endif
  }
}

void playMelody()
{
  for (int thisNote = 0; thisNote < (sizeof(melody) / sizeof(uint32_t)); thisNote++)
  {
    lastTone = (melody[thisNote] >> 24) & 0xFF;
    tone(lastTone);
    delay(melody[thisNote] & 0xFFF);

    lastTone = 0;
    tone(lastTone);
    delay((melody[thisNote] >> 12) & 0xFFF);
  }
}

void pullAdc()
{
  ADCC |= ADCC_START_ADC_CONV | ADCC_ADC_ENABLE;
  while (!(ADCC & ADCC_IS_ADC_CONV_READY))
    ;

  adc = ADCRH / 16;

  ADCC &= ~ADCC_ADC_ENABLE;

#if CONFIG_DEBUG
  itoa(adc, buffer, 10);
  serial_println(buffer);
#endif
}

void main()
{
  PAC |= (1 << BUZZER_BIT);

#if CONFIG_DEBUG
  serial_setup();

  INTRQ = 0;
  __engint();                     // Enable global interrupts

  serial_println("Start");
#endif

  PBC &= ~(1 << ADC_BIT);
  PBPH &= ~(1 << ADC_BIT);
  PBPL &= ~(1 << ADC_BIT);
  PBDIER &= ~(1 << ADC_BIT);

  ADCC = ADCC_CH_AD3_PB3;
  ADCM = ADCM_CLK_SYSCLK_DIV16;

  while (1)
  {
    playMelody();

    _delay_ms(4000);
  }
}

// Startup code - Setup/calibrate system clock
unsigned char _sdcc_external_startup(void)
{

  // Initialize the system clock (CLKMD register) with the IHRC, ILRC, or EOSC clock source and correct divider.
  // The AUTO_INIT_SYSCLOCK() macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC clock source and divider.
  // Alternatively, replace this with the more specific PDK_SET_SYSCLOCK(...) macro from pdk/sysclock.h
  AUTO_INIT_SYSCLOCK();

  // Insert placeholder code to tell EasyPdkProg to calibrate the IHRC or ILRC internal oscillator.
  // The AUTO_CALIBRATE_SYSCLOCK(...) macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC oscillator.
  // Alternatively, replace this with the more specific EASY_PDK_CALIBRATE_IHRC(...) or EASY_PDK_CALIBRATE_ILRC(...) macro from easy-pdk/calibrate.h
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  return 0; // Return 0 to inform SDCC to continue with normal initialization.
}
