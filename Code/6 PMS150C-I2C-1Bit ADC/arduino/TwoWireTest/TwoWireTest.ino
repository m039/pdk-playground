#include <Wire.h>

#define SLAVE_ADDR ((uint8_t)0x2A)

void setup() {
  Wire.begin();            // Нана — мастер
  Wire.setClock(50000);    // слейв-поллер на 16 МГц не тянет 100 кГц
  Serial.begin(115200);
}

void loop() {
  uint8_t vdd, pa4;
  bool ok = false;

  for (uint8_t attempt = 0; attempt < 3 && !ok; attempt++) {
    if (Wire.requestFrom(SLAVE_ADDR, (uint8_t)2) == 2) {
      vdd = Wire.read();               // единица = 100 мВ
      pa4 = Wire.read();
      ok = true;
    }
    // неудача = слейв был в измерении (~0.4 мс) — ретрай
  }

  if (!ok)            Serial.println("no reply");
  else if (!vdd || !pa4) Serial.println("N/A");      // вне шкалы
  else {
    Serial.print("VDD: "); Serial.print(vdd * 100UL); Serial.print(" mV  ");
    Serial.print("PA4: "); Serial.print(pa4 * 100UL); Serial.println(" mV");
  }
  delay(1000);
}
