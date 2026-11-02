/*
  TwoWireTest - Arduino Nano Master for PMS150C I2C 1-Bit ADC Slave.

  Hardware Connections:
    - Nano A4 (SDA) <-> PMS150C PA5 (pin 6)
    - Nano A5 (SCL) <-> PMS150C PA0 (pin 1)
    - Common GND
    - Pull-up resistors: 4.7 kOhm on SDA and SCL to bus VCC (e.g. 3.3V / 5V)

  I2C Specifications:
    - Slave Address: 0x2A
    - Clock Speed: 50 kHz (provides required timing margin for software I2C slave)

  Register Map:
    - Register 0: VDD voltage (units of 100 mV; valid range 2.40V - 5.49V)
    - Register 1: VPA4 voltage (units of 100 mV; valid range 0.031*VDD - 0.50*VDD, e.g. 0.16V - 2.50V at 5V)
    - Register 2: Unique ID (6 bytes)
*/

#include <Wire.h>

const uint8_t DEVICE_ADDR = 0x2A;

void setup() {
  Wire.begin();           // Initialize I2C Master (Nano: SDA = A4, SCL = A5)
  Wire.setClock(50000);   // 50 kHz clock rate for reliable software I2C slave polling
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F(" PMS150C I2C 1-Bit ADC Monitor (Master)"));
  Serial.println(F(" Target Address: 0x2A | Bus Speed: 50 kHz"));
  Serial.println(F("========================================"));
  Serial.println();
}

/*
  Reads 'len' bytes starting from specified register into 'buffer'.
  Returns true on success, false on communication failure.
*/
bool readRegister(uint8_t reg, uint8_t* buffer, uint8_t len) {
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false; // Error addressing register / no ACK from slave
  }

  uint8_t received = Wire.requestFrom(DEVICE_ADDR, (uint8_t)len);
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

  uint8_t vdd = 0;
  uint8_t vpa4 = 0;
  uint8_t id[6] = {0};

  bool okVdd = readRegister(0, &vdd, 1);
  bool okVpa4 = readRegister(1, &vpa4, 1);
  bool okId = readRegister(2, id, sizeof(id));

  // Sample index
  Serial.print(F("[#"));
  if (sampleCount < 10) Serial.print('0');
  Serial.print(sampleCount);
  Serial.print(F("] "));

  // Register 0: VDD (voltage in units of 100 mV)
  Serial.print(F("Reg 0 (VDD): "));
  if (okVdd) {
    if (vdd == 0) {
      Serial.print(F("Out of range"));
    } else {
      float vddVolts = (vdd * 100) / 1000.0f;
      Serial.print(vddVolts, 2);
      Serial.print(F(" V ("));
      Serial.print(vdd * 100);
      Serial.print(F(" mV)"));
    }
  } else {
    Serial.print(F("FAIL [No reply]"));
  }

  Serial.print(F(" | "));

  // Register 1: VPA4 (voltage on PA4 in units of 100 mV)
  Serial.print(F("Reg 1 (VPA4): "));
  if (okVpa4) {
    if (vpa4 == 0) {
      Serial.print(F("Out of range"));
    } else {
      float vpa4Volts = (vpa4 * 100) / 1000.0f;
      Serial.print(vpa4Volts, 2);
      Serial.print(F(" V ("));
      Serial.print(vpa4 * 100);
      Serial.print(F(" mV)"));
    }
  } else {
    Serial.print(F("FAIL [No reply]"));
  }

  Serial.print(F(" | "));

  // Register 2: Unique ID (6 bytes)
  Serial.print(F("Reg 2 (UID): "));
  if (okId) {
    char idStr[20];
    snprintf(idStr, sizeof(idStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             id[0], id[1], id[2], id[3], id[4], id[5]);
    Serial.print(idStr);
  } else {
    Serial.print(F("FAIL [No reply]"));
  }

  Serial.println();
  delay(1000);
}
