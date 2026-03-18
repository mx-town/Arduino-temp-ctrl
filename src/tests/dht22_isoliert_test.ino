// ============================================
// DHT22 Isoliert-Test
// NUR der DHT22 auf D3, sonst NICHTS
// ============================================
//
// DHT22/AM2302 (4 Pins, Gitter zu dir):
//  Pin 1 (links)  → 5V direkt am Nano
//  Pin 2          → D3 direkt am Nano
//  Pin 3          → NICHT VERBINDEN
//  Pin 4 (rechts) → GND direkt am Nano
//
// Pull-Up: 4.7kΩ zwischen Pin 2 und Pin 1
// (oder zwischen D3 und 5V am Nano)
//

#include <DHT.h>

#define DHT_PIN 3
DHT dht(DHT_PIN, DHT22);

void setup() {
  Serial.begin(115200);
  delay(2000);

  dht.begin();

  Serial.println();
  Serial.println(F("=============================="));
  Serial.println(F("  DHT22 ISOLIERT-TEST"));
  Serial.println(F("  Nur DHT22 auf D3"));
  Serial.println(F("=============================="));
  Serial.println();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("NaN — Sensor antwortet nicht"));
  } else {
    Serial.print(F("Temp: "));
    Serial.print(t, 1);
    Serial.print(F(" C  |  Feuchte: "));
    Serial.print(h, 1);
    Serial.println(F(" %RH  <<< OK!"));
  }

  delay(3000);
}
