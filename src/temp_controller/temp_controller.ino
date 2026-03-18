// ============================================
// Temperatur-Controller v7
// DS18B20 (Präzise Temp) + DHT22 (Innen Feuchte)
// Kein Außensensor
// Non-blocking, 350ms Output
// ============================================
//
// PINBELEGUNG:
//  D3  → DHT22/AM2302 Data  (Innen, mit 4.7kΩ Pull-Up nach 5V)
//  D4  → DS18B20 Data       (Präzise Temp, mit 4.7kΩ Pull-Up nach 5V)
//  D5  → LED Blau           (mit 220Ω Vorwiderstand)
//  D6  → LED Grün           (mit 220Ω Vorwiderstand)
//  D7  → LED Rot            (mit 220Ω Vorwiderstand)
//  D8  → BC337 Basis        (mit 1kΩ Basiswiderstand)
//
// DHT22/AM2302 (4 Pins, Gitter zu dir):
//  Pin 1 (links)  → 5V
//  Pin 2          → D3 + 4.7kΩ Pull-Up nach 5V
//  Pin 3          → nicht verbunden
//  Pin 4 (rechts) → GND
//

#include <OneWire.h>
#include <DHT.h>

// ── Pins ──
#define SENSOR_PIN    4   // DS18B20
#define DHT_PIN       3   // DHT22 (Innen)
#define LED_BLAU      5
#define LED_GRUEN     6
#define LED_ROT       7
#define RELAIS_PIN    8

// ── Schwellwerte ──
float tempKalt  = 20.0;
float tempHeiss = 25.0;
float hysterese = 1.0;

// ── DS18B20 ──
OneWire ow(SENSOR_PIN);
byte addr[8] = {0x28, 0x7C, 0x0E, 0xBF, 0x00, 0x00, 0x00, 0x52};

// ── DHT22 ──
DHT dht(DHT_PIN, DHT22);

// ── Status ──
bool luefterAn = false;
String serialBuffer = "";

// ── Non-blocking Timing ──
unsigned long lastConvert = 0;
unsigned long lastOutput = 0;
unsigned long lastDHT = 0;
bool converting = false;

float lastTemp = -999.0;
float lastHumidity = -999.0;
float lastDHTTemp = -999.0;

#define CONVERT_TIME    750
#define OUTPUT_INTERVAL 350
#define DHT_INTERVAL    2500

void starteKonvertierung() {
  if (ow.reset()) {
    ow.select(addr);
    ow.write(0x44, 1);
    lastConvert = millis();
    converting = true;
  }
}

float leseTemperatur() {
  if (!ow.reset()) return -999.0;
  ow.select(addr);
  ow.write(0xBE);
  byte data[9];
  for (int i = 0; i < 9; i++) data[i] = ow.read();
  if (data[0] == 0xFF && data[1] == 0xFF) return -999.0;
  if (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x00) return -999.0;
  int16_t raw = (data[1] << 8) | data[0];
  return (float)raw / 16.0;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BLAU, OUTPUT);
  pinMode(LED_GRUEN, OUTPUT);
  pinMode(LED_ROT, OUTPUT);
  pinMode(RELAIS_PIN, OUTPUT);

  digitalWrite(LED_BLAU, LOW);
  digitalWrite(LED_GRUEN, LOW);
  digitalWrite(LED_ROT, LOW);
  digitalWrite(RELAIS_PIN, HIGH);  // NC: HIGH = aus

  dht.begin();

  delay(1000);
  Serial.println(F("=== Temp Controller v7 ==="));
  Serial.println(F("DS18B20:D4 DHT22:D3(innen)"));
  Serial.println(F("115200 Baud | 350ms Update"));
  Serial.println(F("Befehle: SET:<kalt>:<heiss> | HYST:<wert> | GET | RESET"));
  Serial.println();

  starteKonvertierung();
}

void loop() {
  verarbeiteSerial();
  unsigned long now = millis();

  // DS18B20
  if (converting && (now - lastConvert >= CONVERT_TIME)) {
    float t = leseTemperatur();
    if (t > -50.0 && t < 125.0) lastTemp = t;
    converting = false;
    starteKonvertierung();
  }

  // DHT22 Innen
  if (now - lastDHT >= DHT_INTERVAL) {
    lastDHT = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h)) lastHumidity = h;
    if (!isnan(t)) lastDHTTemp = t;
  }

  // Ausgabe alle 350ms
  if (now - lastOutput >= OUTPUT_INTERVAL) {
    lastOutput = now;

    if (lastTemp < -50.0 || lastTemp > 125.0) {
      Serial.println(F("FEHLER: DS18B20!"));
      blinkAlleWarnung();
      return;
    }

    Serial.print(F("Temperatur: "));
    Serial.print(lastTemp, 1);
    Serial.print(F(" C -> "));

    if (lastTemp < tempKalt) {
      setLEDs(true, false, false);
      Serial.print(F("BLAU"));
    }
    else if (lastTemp <= tempHeiss) {
      setLEDs(false, true, false);
      Serial.print(F("GRUEN"));
    }
    else {
      setLEDs(false, false, true);
      Serial.print(F("ROT"));
    }

    if (!luefterAn && lastTemp > tempHeiss) {
      luefterAn = true;
      digitalWrite(RELAIS_PIN, LOW);
    }
    else if (luefterAn && lastTemp < (tempHeiss - hysterese)) {
      luefterAn = false;
      digitalWrite(RELAIS_PIN, HIGH);
    }

    Serial.print(F(" | Luefter: "));
    Serial.print(luefterAn ? F("AN") : F("AUS"));
    Serial.print(F(" | Kalt:"));
    Serial.print(tempKalt, 1);
    Serial.print(F(" Heiss:"));
    Serial.print(tempHeiss, 1);
    Serial.print(F(" Hyst:"));
    Serial.print(hysterese, 1);

    Serial.print(F(" | Feuchte:"));
    Serial.print(lastHumidity > 0 ? String(lastHumidity, 1) : F("---"));
    Serial.print(F(" DHTTemp:"));
    Serial.println(lastDHTTemp > -50 ? String(lastDHTTemp, 1) : F("---"));
  }
}

void verarbeiteSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      serialBuffer.trim();
      if (serialBuffer.length() > 0) parseBefehl(serialBuffer);
      serialBuffer = "";
    } else {
      if (serialBuffer.length() < 32) serialBuffer += c;
    }
  }
}

void parseBefehl(String cmd) {
  if (cmd.startsWith("SET:")) {
    String params = cmd.substring(4);
    int trenner = params.indexOf(':');
    if (trenner == -1) { Serial.println(F("ERR: SET:<kalt>:<heiss>")); return; }
    float nK = params.substring(0, trenner).toFloat();
    float nH = params.substring(trenner + 1).toFloat();
    if (nK < -20 || nK > 80 || nH < -20 || nH > 80) { Serial.println(F("ERR: Bereich")); return; }
    if (nK >= nH) { Serial.println(F("ERR: Kalt<Heiss")); return; }
    tempKalt = nK; tempHeiss = nH;
    Serial.print(F("OK:")); Serial.print(tempKalt,1); Serial.print(F(":")); Serial.println(tempHeiss,1);
  }
  else if (cmd.startsWith("HYST:")) {
    float nH = cmd.substring(5).toFloat();
    if (nH < 0.1 || nH > 10.0) { Serial.println(F("ERR: Hyst 0.1-10.0")); return; }
    hysterese = nH;
    Serial.print(F("OK:HYST:")); Serial.println(hysterese, 1);
  }
  else if (cmd == "GET") {
    Serial.print(F("CFG:")); Serial.print(tempKalt,1); Serial.print(F(":")); Serial.print(tempHeiss,1);
    Serial.print(F(":")); Serial.println(hysterese,1);
  }
  else if (cmd == "RESET") {
    tempKalt = 20.0; tempHeiss = 25.0; hysterese = 1.0;
    Serial.println(F("OK:20.0:25.0:1.0"));
  }
  else { Serial.print(F("ERR: ")); Serial.println(cmd); }
}

void setLEDs(bool blau, bool gruen, bool rot) {
  digitalWrite(LED_BLAU, blau ? HIGH : LOW);
  digitalWrite(LED_GRUEN, gruen ? HIGH : LOW);
  digitalWrite(LED_ROT, rot ? HIGH : LOW);
}

void blinkAlleWarnung() {
  for (int i = 0; i < 3; i++) {
    setLEDs(true, true, true); delay(100);
    setLEDs(false, false, false); delay(100);
  }
}
