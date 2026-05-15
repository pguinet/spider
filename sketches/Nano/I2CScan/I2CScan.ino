/*
 * I2CScan - Spider Robot (Arduino Nano)
 *
 * Scanne le bus I2C et affiche les adresses qui répondent.
 * Utile pour vérifier que le PCA9685 est correctement câblé
 * (SDA=A4, SCL=A5, VCC=5V logique, GND commun avec le Nano).
 *
 * Adresse par défaut du PCA9685 : 0x40.
 * Si des ponts d'adresse sont soudés, l'adresse change (0x41..0x7F).
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * @dependencies (aucune)
 */

#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println(F("Scan I2C en cours..."));
}

void loop() {
  byte count = 0;

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("  -> peripherique trouve a 0x"));
      if (addr < 16) Serial.print('0');
      Serial.println(addr, HEX);
      count++;
    }
  }

  if (count == 0) {
    Serial.println(F("Aucun peripherique I2C detecte."));
    Serial.println(F("Verifier: SDA=A4, SCL=A5, alim 5V, GND commun, pull-ups."));
  } else {
    Serial.print(F("Total : "));
    Serial.print(count);
    Serial.println(F(" peripherique(s)."));
  }

  Serial.println(F("--- nouvelle passe dans 5s ---"));
  delay(5000);
}
