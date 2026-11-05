/*
  TwoWireTest.ino - Arduino I2C Master Reader for PMS150C UID Chip

  Communicates with a Padauk PMS150C (or PFS154) running the I2C UID Chip firmware.
  The PMS150C emulates an I2C slave (7-bit address 0x2B) and responds with a 6-byte
  Unique ID (UID) from Register 0x00.

  Wiring (Arduino Nano <-> PMS150C SOP-8):
    - A5 (SCL)  <->  Pin 1 (PA0)  [4.7 kΩ pull-up to 5V required]
    - A4 (SDA)  <->  Pin 6 (PA5)  [4.7 kΩ pull-up to 5V required]
    - 5V        <->  Pin 4 (VDD)  [2.4V – 5.5V supply]
    - GND       <->  Pin 2 (GND)  [common ground]

  Deep Sleep & Wake-Up Protocol:
    The PMS150C enters ultra-low-power deep sleep ('stopsys', < 0.5 µA) after 65536 idle cycles.
    To read data:
      1. Master sends a dummy I2C START/STOP ping (Wire.beginTransmission / endTransmission).
      2. Master waits 1 ms (delay(1), range 1–125 ms works) allowing the PMS150C IHRC oscillator
         to start up, stabilize at 8 MHz, and enter the ST_IDLE listening state.
      3. Master sends the register pointer (0x00) and reads the 6-byte UID sequence.
*/

#include <Wire.h>

const uint8_t DEVICE_ADDR = 0x2B;
const uint8_t REG_UID     = 0x00;
const uint8_t UID_LENGTH  = 6;

void setup() {
  Wire.begin();           // Initialize I2C Master (Nano: SDA = A4, SCL = A5)
  Wire.setClock(75000);   // 50 kHz clock rate for reliable software I2C slave polling
  Serial.begin(115200);

  while (!Serial && millis() < 2000); // Wait for serial port on USB boards

  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" PMS150C UID Chip Reader (Arduino I2C Master)"));
  Serial.println(F(" Target Address : 0x2B (Register 0x00: 6-byte UID)"));
  Serial.println(F(" Bus Clock      : 50 kHz | Serial: 115200 baud"));
  Serial.println(F(" Wake-up Delay  : 1 ms (allows IHRC 8 MHz to settle)"));
  Serial.println(F(" Required Pins  : A4 (SDA), A5 (SCL) + 4.7k pull-ups"));
  Serial.println(F("=================================================="));
  Serial.println();
}

/*
  Reads 'len' bytes starting from specified register into 'buffer'.
  Parameters:
    reg    - Target register index (0x00 for UID)
    buffer - Output byte buffer
    len    - Number of bytes to read
  Returns true on success, false on communication failure.
*/
bool readRegister(uint8_t reg, uint8_t* buffer, uint8_t len) {
  // 1. Wake-up ping: toggle the bus to wake the slave from stopsys
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.endTransmission(true);
  delay(2); // 1 - 125 ms works: allows 8 MHz IHRC to wake and stabilize

  // 2. Transmit target register pointer
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false; // Error addressing register / no ACK from slave
  }

  // 3. Request data bytes from slave
  uint8_t received = Wire.requestFrom(DEVICE_ADDR, len);
  if (received != len) {
    return false; // Did not receive expected number of bytes
  }

  for (uint8_t i = 0; i < len; i++) {
    buffer[i] = Wire.read();
  }
  return true;
}

void loop() {
  static uint32_t sampleCount = 0;
  sampleCount++;

  uint8_t id[UID_LENGTH] = {0};

  bool okId = readRegister(REG_UID, id, sizeof(id));

  // Sample index and uptime
  Serial.print(F("[#"));
  if (sampleCount < 10) Serial.print('0');
  Serial.print(sampleCount);
  Serial.print(F(" | "));
  Serial.print(millis() / 1000);
  Serial.print(F("s] Reg 0 (UID): "));

  if (okId) {
    char idStr[24];
    snprintf(idStr, sizeof(idStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             id[0], id[1], id[2], id[3], id[4], id[5]);
    Serial.print(idStr);
    Serial.println(F("  [OK]"));
  } else {
    Serial.println(F("FAIL [No reply]"));
    Serial.println(F("        -> Check 4.7k pull-up resistors on A4/A5 and PMS150C power."));
  }

  delay(1000);
}
