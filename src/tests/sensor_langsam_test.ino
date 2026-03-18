// DS18B20 Test mit langer Wartezeit
// Fuer langsame / gefaelschte Sensoren

#include <OneWire.h>
#include <DallasTemperature.h>

#define SENSOR_PIN 4

OneWire oneWire(SENSOR_PIN);
DallasTemperature sensor(&oneWire);

void setup() {
  Serial.begin(9600);
  delay(1000);
  sensor.begin();
  
  // Warte NICHT auf Konvertierung automatisch
  sensor.setWaitForConversion(false);

  Serial.println(F("=== DS18B20 Langsam-Test ==="));
  Serial.print(F("Sensoren: "));
  Serial.println(sensor.getDeviceCount());
  
  // Aufloesung auf 12-bit setzen
  sensor.setResolution(12);
  Serial.println(F("Aufloesung: 12-bit"));
  Serial.println();
}

void loop() {
  Serial.print(F("Starte Messung... "));
  sensor.requestTemperatures();
  
  // 2 Sekunden warten (normal reichen 750ms)
  delay(2000);
  
  float temp = sensor.getTempCByIndex(0);
  
  if (temp == -127.0) {
    Serial.println(F("-127 (FEHLER)"));
    Serial.println(F("Teste mit Raw-Daten..."));
    
    // Raw Daten direkt vom OneWire Bus lesen
    byte addr[8];
    if (oneWire.search(addr)) {
      Serial.print(F("  Adresse: "));
      for (int i = 0; i < 8; i++) {
        if (addr[i] < 16) Serial.print("0");
        Serial.print(addr[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Family code pruefen
      if (addr[0] == 0x28) {
        Serial.println(F("  Family: 0x28 = DS18B20 (echt)"));
      } else if (addr[0] == 0x10) {
        Serial.println(F("  Family: 0x10 = DS18S20 (alt)"));
      } else {
        Serial.print(F("  Family: 0x"));
        Serial.print(addr[0], HEX);
        Serial.println(F(" = UNBEKANNT / FAELSCHUNG?"));
      }
      
      // Scratchpad lesen
      oneWire.reset();
      oneWire.select(addr);
      oneWire.write(0x44); // Convert
      delay(2000);
      
      oneWire.reset();
      oneWire.select(addr);
      oneWire.write(0xBE); // Read Scratchpad
      
      Serial.print(F("  Scratchpad: "));
      byte data[9];
      for (int i = 0; i < 9; i++) {
        data[i] = oneWire.read();
        if (data[i] < 16) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Temperatur manuell berechnen
      int16_t raw = (data[1] << 8) | data[0];
      float celsius = (float)raw / 16.0;
      Serial.print(F("  Raw Temp: "));
      Serial.print(celsius);
      Serial.println(F(" C"));
      
      if (celsius > -50 && celsius < 100) {
        Serial.println(F("  >>> SENSOR FUNKTIONIERT! <<<"));
        Serial.println(F("  Library hat Problem, Raw-Daten OK"));
      }
    }
    oneWire.reset_search();
  } else {
    Serial.print(temp, 1);
    Serial.println(F(" C  >>> OK! <<<"));
  }
  
  Serial.println();
  delay(3000);
}
