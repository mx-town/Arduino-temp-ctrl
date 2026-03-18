// DHT11 Pin-Scanner
// Testet mehrere Pins
#include <DHT.h>

void testDHT(int pin) {
  DHT d(pin, DHT11);
  d.begin();
  delay(2500);  // DHT11 braucht Startzeit

  float h = d.readHumidity();
  float t = d.readTemperature();

  Serial.print(F("  D"));
  Serial.print(pin);
  Serial.print(F(": "));

  if (!isnan(h) && !isnan(t)) {
    Serial.print(t, 1);
    Serial.print(F(" C  "));
    Serial.print(h, 1);
    Serial.print(F(" %RH  <<<< GEFUNDEN!"));
  } else {
    Serial.print(F("NaN"));
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println(F("=== DHT11 Pin-Scanner ==="));
  Serial.println(F("Teste alle Pins... (dauert ca. 30s)"));
  Serial.println();

  testDHT(2);
  testDHT(3);
  testDHT(4);
  testDHT(5);
  testDHT(6);
  testDHT(7);
  testDHT(8);
  testDHT(9);
  testDHT(10);
  testDHT(11);
  testDHT(12);

  Serial.println();
  Serial.println(F("Scan fertig."));
  Serial.println(F("Falls alles NaN:"));
  Serial.println(F("  1. Sensor defekt?"));
  Serial.println(F("  2. VCC und GND vertauscht?"));
  Serial.println(F("  3. Anderes DHT11 Modul probieren"));
  Serial.println(F("RESET fuer neuen Scan"));
}

void loop() {
  delay(10000);
}
