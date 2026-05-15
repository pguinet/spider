/*
 * Blink - Spider Robot (Arduino Nano)
 *
 * Clignote rapidement la LED intégrée (pin 13). Utilitaire pour identifier
 * physiquement un Nano parmi plusieurs branchés en USB.
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * @dependencies (aucune)
 */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
