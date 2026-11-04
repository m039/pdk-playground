
#include <Wire.h>

const uint8_t DEVICE_ADDR = 0x2B;

void setup() {
  Wire.begin();           // Initialize I2C Master (Nano: SDA = A4, SCL = A5)
  Wire.setClock(50000);   // 50 kHz clock rate for reliable software I2C slave polling
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F(" PMS150C UID Chip (Master)"));
  Serial.println(F(" Target Address: 0x2B | Bus Speed: 50 kHz"));
  Serial.println(F("========================================"));
  Serial.println();
}

/*
  Reads 'len' bytes starting from specified register into 'buffer'.
  Returns true on success, false on communication failure.
*/
bool readRegister(uint8_t reg, uint8_t* buffer, uint8_t len) {
  // Wake-Up the chip.
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.endTransmission(true);
  delay(1); // 1 - 125ms works

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

  uint8_t id[6] = {0};

  bool okId = readRegister(0, id, sizeof(id));

  // Sample index
  Serial.print(F("[#"));
  if (sampleCount < 10) Serial.print('0');
  Serial.print(sampleCount);
  Serial.print(F("] "));

  // Register 2: Unique ID (6 bytes)
  Serial.print(F("Reg 0 (UID): "));
  if (okId) {
    char idStr[20];
    snprintf(idStr, sizeof(idStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             id[0], id[1], id[2], id[3], id[4], id[5]);
    Serial.print(idStr);
  } else {
    Serial.print(F("FAIL [No reply]"));
  }

  Serial.println();
  delay(3000);
}
