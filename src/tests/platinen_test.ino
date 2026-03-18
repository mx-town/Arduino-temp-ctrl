// ============================================
// Platinen-Test OHNE Sensor
// Testet LEDs und Relais/BC337
// ============================================

#define LED_BLAU   5
#define LED_GRUEN  6
#define LED_ROT    7
#define RELAIS_PIN 8

void setup() {
  Serial.begin(9600);
  pinMode(LED_BLAU, OUTPUT);
  pinMode(LED_GRUEN, OUTPUT);
  pinMode(LED_ROT, OUTPUT);
  pinMode(RELAIS_PIN, OUTPUT);

  Serial.println(F("=== Platinen-Test (ohne Sensor) ==="));
  Serial.println(F("Teste LEDs und Relais..."));
  Serial.println();
}

void loop() {
  // LED Blau
  Serial.println(F("LED BLAU an"));
  digitalWrite(LED_BLAU, HIGH);
  delay(1000);
  digitalWrite(LED_BLAU, LOW);

  // LED Gruen
  Serial.println(F("LED GRUEN an"));
  digitalWrite(LED_GRUEN, HIGH);
  delay(1000);
  digitalWrite(LED_GRUEN, LOW);

  // LED Rot
  Serial.println(F("LED ROT an"));
  digitalWrite(LED_ROT, HIGH);
  delay(1000);
  digitalWrite(LED_ROT, LOW);

  // Relais (nur testen wenn 24V Netzteil dran!)
  Serial.println(F("RELAIS an (BC337 schaltet)"));
  digitalWrite(RELAIS_PIN, HIGH);
  delay(2000);

  Serial.println(F("RELAIS aus"));
  digitalWrite(RELAIS_PIN, LOW);
  delay(2000);

  Serial.println(F("--- Durchlauf fertig ---"));
  Serial.println();
}
