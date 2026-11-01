/*
  TwoWireTest - Master reader for Padauk PMS150C (1-bit ADC + I2C slave).
  Padauk I2C Slave Address: 0x2A

  Registers:
    Reg 0: VDD (1 byte, 1 unit = 100 mV, 0 = out of range / N/A)
    Reg 1: VPA4 (1 byte, 1 unit = 100 mV, 0 = out of range / N/A)
    Reg 2: 8-byte Unique Device ID (compile-time defined in firmware)
*/

#include <Wire.h>

#define SLAVE_ADDR ((uint8_t)0x2A)

// Helper to write register address and read N bytes from slave
bool readRegister(uint8_t reg, uint8_t *buffer, uint8_t len) {
  for (uint8_t attempt = 0; attempt < 5; attempt++) {
    // 1. Select register
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) { // repeated START
      // Fallback with STOP then START if repeated start was NACKed
      Wire.beginTransmission(SLAVE_ADDR);
      Wire.write(reg);
      if (Wire.endTransmission(true) != 0) {
        delay(2);
        continue;
      }
    }

    // 2. Read requested bytes
    if (Wire.requestFrom(SLAVE_ADDR, len) == len) {
      for (uint8_t i = 0; i < len; i++) {
        buffer[i] = Wire.read();
      }
      return true;
    }

    // Measurement on Padauk takes ~0.4ms; retry if slave was busy
    delay(2);
  }
  return false;
}

void setup() {
  Wire.begin();         // Arduino Nano is I2C Master
  Wire.setClock(50000); // 50 kHz clock for software-polled slave on 16MHz/8MHz
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println(F("=== Padauk PMS150C I2C Register Reader ==="));
  Serial.println(F("Reg 0: VDD | Reg 1: VPA4 | Reg 2: 8-byte UID"));
}

void loop() {
  uint8_t vdd = 0;
  uint8_t pa4 = 0;
  uint8_t uid[8] = {0};

  bool ok_vdd = readRegister(0, &vdd, 1);
  bool ok_pa4 = readRegister(1, &pa4, 1);
  bool ok_uid = readRegister(2, uid, 8);

  if (!ok_vdd && !ok_pa4 && !ok_uid) {
    Serial.println(F("No reply from Padauk PMS150C (0x2A)"));
  } else {
    // Register 0: VDD
    Serial.print(F("Reg 0 (VDD): "));
    if (ok_vdd && vdd != 0) {
      Serial.print(vdd * 100UL);
      Serial.print(F(" mV (raw: 0x"));
      if (vdd < 16) Serial.print('0');
      Serial.print(vdd, HEX);
      Serial.print(F(")"));
    } else {
      Serial.print(F("N/A"));
    }

    // Register 1: VPA4
    Serial.print(F(" | Reg 1 (VPA4): "));
    if (ok_pa4 && pa4 != 0) {
      Serial.print(pa4 * 100UL);
      Serial.print(F(" mV (raw: 0x"));
      if (pa4 < 16) Serial.print('0');
      Serial.print(pa4, HEX);
      Serial.print(F(")"));
    } else {
      Serial.print(F("N/A"));
    }

    // Register 2: 8-byte UID
    Serial.print(F(" | Reg 2 (UID): "));
    if (ok_uid) {
      for (uint8_t i = 0; i < 8; i++) {
        if (i > 0) Serial.print(':');
        if (uid[i] < 16) Serial.print('0');
        Serial.print(uid[i], HEX);
      }
    } else {
      Serial.print(F("N/A"));
    }
    Serial.println();
  }

  delay(1000);
}
