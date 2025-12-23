#ifndef __CONFIG__
#define __CONFIG__

#define BUZZER_BIT 3 // PA3 (TM2PWM)

#define BUTTON_BIT 4 // PA4

#define isButtonActive()    !(PA & (1 << BUTTON_BIT))

#define sleep() __asm stopsys __endasm;

#define buzzerOn()\
    PAC |= (1 << BUZZER_BIT);\
    TM2C = (uint8_t)(TM2C_MODE_PERIOD | TM2C_OUT_PA3 | TM2C_CLK_IHRC);

#define buzzerOff()\
    PAC &= ~(1 << BUZZER_BIT);\
    TM2C = 0;

#define buttonSetup()\
  PADIER |= (1 << BUTTON_BIT);\
  PAPH |= (1 << BUTTON_BIT);

#endif