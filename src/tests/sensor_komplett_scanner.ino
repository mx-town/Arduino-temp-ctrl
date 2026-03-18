// ============================================
// DS18B20 KOMPLETT-Scanner
// Testet ALLE Pins D2-D13 und A0-A5
// ============================================

#include <OneWire.h>
#include <DallasTemperature.h>

void testPin(int pin, const char* name) {
  OneWire ow(pin);
  DallasTemperature sensor(&ow);
  sensor.begin();
  
  int count = sensor.getDeviceCount();
  Serial.print(F("  "));
  Serial.print(name);
  Serial.print(F(": "));
  
  if (count > 0) {
    Serial.print(count);
    Serial.print(F(" Sensor(en)! <<<< GEFUNDEN!"));
    sensor.requestTemperatures();
    float temp = sensor.getTempCByIndex(0);
    Serial.print(F("  Temp: "));
    Serial.print(temp, 1);
    Serial.print(F(" C"));
  } else {
    Serial.print(F("-"));
  }
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F("  DS18B20 KOMPLETT-Scanner"));
  Serial.println(F("================================"));
  Serial.println();
  Serial.println(F("Scanne Digital-Pins..."));
  
  testPin(2, "D2 ");
  testPin(3, "D3 ");
  testPin(4, "D4 ");
  testPin(5, "D5 ");
  testPin(6, "D6 ");
  testPin(7, "D7 ");
  testPin(8, "D8 ");
  testPin(9, "D9 ");
  testPin(10, "D10");
  testPin(11, "D11");
  testPin(12, "D12");
  testPin(13, "D13");
  
  Serial.println();
  Serial.println(F("Scanne Analog-Pins..."));
  
  testPin(A0, "A0 ");
  testPin(A1, "A1 ");
  testPin(A2, "A2 ");
  testPin(A3, "A3 ");
  testPin(A4, "A4 ");
  testPin(A5, "A5 ");
  
  Serial.println();
  Serial.println(F("================================"));
  Serial.println(F("Scan fertig."));
  Serial.println();
  Serial.println(F("Nichts gefunden? Dann:"));
  Serial.println(F("  1. Kabelfarben tauschen probieren"));
  Serial.println(F("  2. Pull-Up sitzt auf DATA-Leitung?"));
  Serial.println(F("  3. Sensor evtl. defekt"));
  Serial.println(F("  4. RESET druecken fuer neuen Scan"));
}

void loop() {
  delay(10000);
}
