/*
 * ServoCenter - Spider Robot (Arduino Nano)
 *
 * Force les 16 canaux du PCA9685 à la position neutre (~90°, ~1.5 ms).
 * Sketch utilisé pour le réglage mécanique des palonniers (servo horns) :
 *   1. Uploader ce sketch avec les pattes encore démontées (ou horns desserrés),
 *   2. Tous les servos se placent à 90°,
 *   3. (Re)monter chaque palonnier dans la position neutre voulue,
 *   4. Serrer la vis du palonnier.
 *
 * ⚠️ V+ du PCA9685 doit être alimenté (alim servo séparée 5-6V, masse commune).
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * @dependencies Adafruit PWM Servo Driver Library, Adafruit BusIO
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// PWM ticks à 50 Hz (4096 ticks/période de 20 ms)
//   ~1.0 ms ≈ 205  -> 0°
//   ~1.5 ms ≈ 307  -> 90°  (neutre)
//   ~2.0 ms ≈ 410  -> 180°
const uint16_t SERVO_CENTER = 307;
const uint16_t SERVO_FREQ_HZ = 50;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  Serial.println(F("=== ServoCenter : tous les servos a 90 deg ==="));
  Serial.print(F("PCA9685 a l'adresse 0x"));
  Serial.println(0x40, HEX);
  Serial.print(F("Ticks neutres envoyes : "));
  Serial.println(SERVO_CENTER);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  for (uint8_t ch = 0; ch < 16; ch++) {
    pwm.setPWM(ch, 0, SERVO_CENTER);
  }

  Serial.println(F("Tous les canaux maintenus a 90 deg."));
  Serial.println(F("Ajuste les palonniers maintenant, puis serre les vis."));
}

void loop() {
  // Rien à faire : les valeurs envoyées au setup() sont conservées par le PCA9685.
}
