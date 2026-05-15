/*
 * NamedServoTest - Spider Robot (Arduino Nano)
 *
 * Anime chaque articulation par son nom logique (FR_COXA, FR_FEMUR, ...)
 * en utilisant le mapping de SpiderConfig.h. Sert à valider que le câblage
 * et le mapping sont cohérents : la patte avant droite doit bouger quand
 * "FR_..." s'affiche, etc.
 *
 * Ordre du test : FR -> FL -> RL -> RR, chacun COXA -> FEMUR -> TIBIA.
 * Oscillation douce ±~15° autour du neutre.
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * Compilation (un seul des deux) :
 *   --build-property "build.extra_flags=-DROBOT_PHILIPPE"
 *   --build-property "build.extra_flags=-DROBOT_LAURENT"
 *
 * @dependencies Adafruit PWM Servo Driver Library, Adafruit BusIO
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SpiderConfig.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDR);

const char* const SERVO_NAMES[12] = {
  "FR_COXA", "FR_FEMUR", "FR_TIBIA",
  "FL_COXA", "FL_FEMUR", "FL_TIBIA",
  "RL_COXA", "RL_FEMUR", "RL_TIBIA",
  "RR_COXA", "RR_FEMUR", "RR_TIBIA"
};

const uint16_t SERVO_AMP = 35;   // ±35 ticks ≈ ±15° autour du neutre
const uint16_t HOLD_MS   = 500;
const uint8_t  CYCLES    = 3;

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

#if defined(ROBOT_PHILIPPE)
  Serial.println(F("=== NamedServoTest : Philippe ==="));
#elif defined(ROBOT_LAURENT)
  Serial.println(F("=== NamedServoTest : Laurent ==="));
#endif

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  // Mettre les 12 servos utilisés au neutre
  for (uint8_t i = 0; i < 12; i++) {
    pwm.setPWM(SERVO_CHANNELS[i], 0, SERVO_TICK_CENTER);
  }
  delay(1000);
  Serial.println(F("Tous au neutre. Debut du test."));
}

void sweepNamed(uint8_t idx) {
  uint8_t ch = SERVO_CHANNELS[idx];

  Serial.print(F(">>> "));
  Serial.print(SERVO_NAMES[idx]);
  Serial.print(F(" (canal "));
  Serial.print(ch);
  Serial.println(F(")"));

  for (uint8_t i = 0; i < CYCLES; i++) {
    pwm.setPWM(ch, 0, SERVO_TICK_CENTER + SERVO_AMP);
    delay(HOLD_MS);
    pwm.setPWM(ch, 0, SERVO_TICK_CENTER - SERVO_AMP);
    delay(HOLD_MS);
  }
  pwm.setPWM(ch, 0, SERVO_TICK_CENTER);
  delay(300);
}

void loop() {
  for (uint8_t i = 0; i < 12; i++) {
    sweepNamed(i);
  }
  Serial.println(F("--- Tour complet. Pause 3s avant nouvelle passe. ---"));
  delay(3000);
}
