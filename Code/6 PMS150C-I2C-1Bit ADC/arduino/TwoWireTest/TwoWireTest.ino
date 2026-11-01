#include <Wire.h>

const uint8_t DEVICE_ADDR = 0x2A;

void setup() {
  Wire.begin();            // Nano: SDA=A4, SCL=A5
  Wire.setClock(50000);    // 50 кГц — если слейв не тянет 100 кГц
  Serial.begin(115200);
  Serial.println("==== Start ====");
}

bool readRegister(uint8_t reg, uint8_t* buffer, uint8_t len) {
  Wire.beginTransmission(DEVICE_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return false;          // ошибка при записи адреса регистра
  }

  uint8_t received = Wire.requestFrom(DEVICE_ADDR, (uint8_t)len);
  if (received != len) {
    return false;          // получили не то количество байт
  }

  for (uint8_t i = 0; i < len; i++) {
    buffer[i] = Wire.read();
  }
  return true;
}

void loop() {
  uint8_t vdd, vpa4;
  uint8_t id[6];
  bool okVdd = readRegister(0, &vdd, 1);
  bool okVpa4 = readRegister(1, &vpa4, 1);
  bool okId = readRegister(2, id, 6);

  Serial.print("Reg0: ");
  if (okVdd) {
    Serial.print(vdd * 100);
    Serial.print(" mV");
  } else {
    Serial.print("N/A");
  }

  Serial.print(" | ");
  Serial.print("Reg1: ");

  if (okVpa4) {
    Serial.print(vpa4 * 100);
    Serial.print(" mV");
  } else {
    Serial.print("N/A");
  }

  Serial.print(" | ");
  Serial.print("Reg2: ");

  if (okId) {
    char buffer[32];
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", id[0], id[1], id[2], id[3], id[4], id[5]);
    Serial.print(buffer);
  } else {
    Serial.print("N/A");
  }

  Serial.println();
  delay(1000);
}
