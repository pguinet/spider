/*
 * ServoSweep - Spider Robot (Arduino Nano)
 *
 * Identifie le câblage des 12 servos sur le PCA9685.
 * Balaye les 16 canaux du driver un par un, en faisant osciller doucement
 * le servo autour de sa position neutre (~±15°), pendant quelques secondes.
 * Affiche le canal en cours dans le moniteur série : tu notes l'articulation
 * qui bouge (patte AvG/AvD/ArG/ArD, hanche/cuisse/genou).
 *
 * ⚠️ Sweep volontairement étroit pour ne pas forcer une patte déjà montée.
 *
 * Câblage attendu :
 *   PCA9685 SDA  -> Nano A4
 *   PCA9685 SCL  -> Nano A5
 *   PCA9685 VCC  -> Nano 5V (logique)
 *   PCA9685 GND  -> Nano GND (masse commune indispensable)
 *   PCA9685 V+   -> alim servo séparée 5-6V (BEC ou pack)
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * @dependencies Adafruit PWM Servo Driver Library, Adafruit BusIO
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Adresse I2C par défaut du PCA9685 (vérifier d'abord avec I2CScan)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// PWM ticks pour servo analogique à 50 Hz (4096 ticks/période de 20 ms)
//   ~1.0 ms ≈ 205   (0°)
//   ~1.5 ms ≈ 307   (90°, neutre)
//   ~2.0 ms ≈ 410   (180°)
const uint16_t SERVO_CENTER = 307;
const uint16_t SERVO_AMP    = 35;   // ±35 ticks ≈ ±15° autour du neutre
const uint16_t SERVO_FREQ_HZ = 50;

const uint8_t  CHANNEL_MIN = 0;
const uint8_t  CHANNEL_MAX = 15;
const uint16_t HOLD_MS     = 500;   // durée à chaque extrême
const uint8_t  CYCLES      = 3;     // nombre d'allers-retours par canal

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  Serial.println(F("=== ServoSweep : identification du cablage ==="));
  Serial.print(F("PCA9685 a l'adresse 0x"));
  Serial.println(0x40, HEX);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  // Tous les canaux au neutre au démarrage
  for (uint8_t ch = CHANNEL_MIN; ch <= CHANNEL_MAX; ch++) {
    pwm.setPWM(ch, 0, SERVO_CENTER);
  }
  delay(1000);
  Serial.println(F("Tous les canaux mis au neutre. Debut du balayage."));
}

void sweepChannel(uint8_t ch) {
  Serial.print(F(">>> Canal "));
  Serial.print(ch);
  Serial.println(F(" en mouvement"));

  for (uint8_t i = 0; i < CYCLES; i++) {
    pwm.setPWM(ch, 0, SERVO_CENTER + SERVO_AMP);
    delay(HOLD_MS);
    pwm.setPWM(ch, 0, SERVO_CENTER - SERVO_AMP);
    delay(HOLD_MS);
  }
  pwm.setPWM(ch, 0, SERVO_CENTER);
  delay(300);
}

void loop() {
  for (uint8_t ch = CHANNEL_MIN; ch <= CHANNEL_MAX; ch++) {
    sweepChannel(ch);
  }

  Serial.println(F("--- Tour complet termine. Pause 3s avant nouvelle passe. ---"));
  delay(3000);
}
