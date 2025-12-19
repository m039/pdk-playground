Example from https://github.com/free-pdk/free-pdk-examples/tree/master/BlinkLED

# BlinkLED Example

This example demonstrates just about the simplest thing you can do: it blinks an LED, on for one second, then off for one second, repeatedly.

Note: This example uses a timing loop to delay for one second.
See the [BlinkLED_WithIRQ](../BlinkLED_WithIRQ) example for a timer based approach. 

> _**Inspiration**: This example was inspired by the public domain [Blink](https://www.arduino.cc/en/Tutorial/Blink) example from Arduino._

### Hardware Circuit
By default, the LED is placed on the PA4 pin* (Port A, Bit 4) with a current sink configuration.

This means the negative leg (or cathode) of the LED is connected to the digital pin of the IC, and the positive leg (or anode) of the LED is connected through a current limiting resistor to VDD.
- When the digital pin is LOW, current will flow through the LED and it will light up.
- When the digital pin is HIGH, no current will flow and the LED will turn off.

>_*Note: Please consult the pinout for the specific microcontroller package used to identify the correct physical pin._

### Compatibility
This example should run on every currently known Padauk microcontroller that is supported by SDCC and the Easy PDK Programmer.
A device specific include file (pdk/device/*.h) may need to be supplied for less common devices.

### Build Stats
- Code Size: 87 words (174 bytes)
- RAM usage: 7 bytes + stack
  - All 7 bytes are used as method call parameters for the timing loop routines defined in the [delay.h](../include/delay.h) include file.
