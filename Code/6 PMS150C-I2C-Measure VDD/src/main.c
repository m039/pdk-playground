/*
  BlinkLED

  Turns an LED on for one second, then off for one second, repeatedly.
  Uses a timing loop for delays.
*/

#define DEBUG 1

#include <pdk/device.h>
#include "auto_sysclock.h"
#include "delay.h"

#if DEBUG != 0
#include "serial.h"
#include <stdlib.h>
#endif

// LED is placed on the PA4 pin (Port A, Bit 4) with a current sink configuration
#define LED_BIT             4

// LED is active low (current sink), so define helpers for better readability below
#define turnLedOn()         PA &= ~(1 << LED_BIT)
#define turnLedOff()        PA |= (1 << LED_BIT)

int8_t get_nvdd();

#if DEBUG != 0
void interrupt(void) __interrupt(0) {
  if (INTRQ & INTRQ_TM2) {        // TM2 interrupt request?
    INTRQ &= ~INTRQ_TM2;          // Mark TM2 interrupt request processed
    serial_irq_handler();         // Process next Serial Bit
  }
}

void print_vdd(uint16_t number) {
  const uint8_t *p = "VDD: ";
  while (*p != 0) {
    putchar(*p);
    p++;
  }

  char buffer[6];
  __itoa(number, buffer, 10);
  puts(buffer);
}

uint16_t get_vdd() {
  int8_t nvdd = get_nvdd();

  if (nvdd < 0) {
    return -1;
  } else {
    #define VDD(n) (120 * 40 / (8 + n + 1)) // Case 3
    // #define VDD(n) (120 * 32 / (n + 1)) // Case 4
    static const uint16_t vdds[] = {
      VDD(0),
      VDD(1),
      VDD(2),
      VDD(3),
      VDD(4),
      VDD(5),
      VDD(6),
      VDD(7),
      VDD(8),
      VDD(9),
      VDD(10),
      VDD(11),
      VDD(12),
      VDD(13),
      VDD(14),
      VDD(15),
    };
    #undef VDD
    return vdds[nvdd];
  }
}
#endif

int8_t get_nvdd() {
 GPCC = (uint8_t)(GPCC_COMP_ENABLE | GPCC_COMP_MINUS_BANDGAP_1V2 | GPCC_COMP_PLUS_VINT_R);

  for (int8_t i = 0; i < 16; i++) {
    GPCS = (1 << 5) | (0 << 4) | i; // Case 3
    _delay_us(1);

    if (GPCC & GPCC_COMP_RESULT_POSITIVE) {
      return i;
    }
  }

  return -1;
}

#define TO_NVDD(x) (int8_t)(1.0 / x * 120.0 * 40.0 - 8.0 - 1.0)

// Main program
void main() {

  #if DEBUG != 0
  serial_setup();

  INTRQ = 0;
  __engint();                     // Enable global interrupts
  #endif

  // Initialize hardware
  PAC |= (1 << LED_BIT);          // Set LED as output (all pins are input by default)
  turnLedOff();

  // Main processing loop
  while (1) {
    
    #if DEBUG != 0
    int8_t nvdd = get_nvdd();
    if (nvdd <= TO_NVDD(400) && nvdd >= TO_NVDD(600)) {
      serial_println("Voltage in 4.00V to 6.00V");
    } else {
      serial_println("Voltage out of range");
    }
    print_vdd(get_vdd());
    #endif

    turnLedOn();
    _delay_ms(1000);
    turnLedOff();
    _delay_ms(1000);
  }
}

// Startup code - Setup/calibrate system clock
unsigned char _sdcc_external_startup(void) {

  // Initialize the system clock (CLKMD register) with the IHRC, ILRC, or EOSC clock source and correct divider.
  // The AUTO_INIT_SYSCLOCK() macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC clock source and divider.
  // Alternatively, replace this with the more specific PDK_SET_SYSCLOCK(...) macro from pdk/sysclock.h
  AUTO_INIT_SYSCLOCK();

  // Insert placeholder code to tell EasyPdkProg to calibrate the IHRC or ILRC internal oscillator.
  // The AUTO_CALIBRATE_SYSCLOCK(...) macro uses F_CPU (defined in the Makefile) to choose the IHRC or ILRC oscillator.
  // Alternatively, replace this with the more specific EASY_PDK_CALIBRATE_IHRC(...) or EASY_PDK_CALIBRATE_ILRC(...) macro from easy-pdk/calibrate.h
  AUTO_CALIBRATE_SYSCLOCK(TARGET_VDD_MV);

  return 0;   // Return 0 to inform SDCC to continue with normal initialization.
}
