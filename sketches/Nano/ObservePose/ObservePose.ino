/*
 * ObservePose - Spider Robot (Arduino Nano)
 *
 * Pose statique distinctive pour vérifier le sens de chaque articulation :
 *   - Pattes avant (FR, FL) : COXA pointant vers l'avant
 *   - Pattes arrière (RL, RR) : COXA pointant vers l'arrière
 *   - FEMUR : tous vers le haut (cuisse levée)
 *   - TIBIA : tous rentrés vers l'intérieur (pied vers le corps)
 *
 * Au boot, on ignore où sont les servos : on part de 90° et on interpole
 * doucement vers la pose cible en 2 s.
 *
 * Si une articulation va dans le sens INVERSE de ce qui est décrit ci-dessus,
 * inverser la valeur (60 -> 120 ou 120 -> 60) dans le bloc "Angles cibles".
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 * Compilation : --build-property "build.extra_flags=-DROBOT_LAURENT"  (ou PHILIPPE)
 *
 * @dependencies Adafruit PWM Servo Driver Library, Adafruit BusIO
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SpiderConfig.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDR);

// ============================================================
// Angles cibles — hypothèse miroir gauche/droite + inversion avant/arrière
// (à ajuster selon ce qui est observé sur Laurent)
// ============================================================
// Patte avant droite (FR) — COXA vers l'avant, FEMUR haut, TIBIA rentré
const uint8_t FR_COXA_DEG  = 120;
const uint8_t FR_FEMUR_DEG =  60;
const uint8_t FR_TIBIA_DEG =  60;

// Patte avant gauche (FL) — miroir gauche/droite
const uint8_t FL_COXA_DEG  =  60;
const uint8_t FL_FEMUR_DEG = 120;
const uint8_t FL_TIBIA_DEG = 120;

// Patte arrière gauche (RL) — sur Laurent, FEMUR/TIBIA dans le même sens que FL
const uint8_t RL_COXA_DEG  = 120;
const uint8_t RL_FEMUR_DEG = 120;
const uint8_t RL_TIBIA_DEG = 120;

// Patte arrière droite (RR) — sur Laurent, FEMUR/TIBIA dans le même sens que FR
const uint8_t RR_COXA_DEG  =  60;
const uint8_t RR_FEMUR_DEG =  60;
const uint8_t RR_TIBIA_DEG =  60;

// Tableau aligné sur SERVO_CHANNELS (ordre FR, FL, RL, RR × COXA, FEMUR, TIBIA)
const uint8_t TARGET_DEG[12] = {
  FR_COXA_DEG, FR_FEMUR_DEG, FR_TIBIA_DEG,
  FL_COXA_DEG, FL_FEMUR_DEG, FL_TIBIA_DEG,
  RL_COXA_DEG, RL_FEMUR_DEG, RL_TIBIA_DEG,
  RR_COXA_DEG, RR_FEMUR_DEG, RR_TIBIA_DEG
};

const char* const SERVO_NAMES[12] = {
  "FR_COXA", "FR_FEMUR", "FR_TIBIA",
  "FL_COXA", "FL_FEMUR", "FL_TIBIA",
  "RL_COXA", "RL_FEMUR", "RL_TIBIA",
  "RR_COXA", "RR_FEMUR", "RR_TIBIA"
};

const uint16_t MOVE_DURATION_MS = 2000;
const uint8_t  STEP_DELAY_MS    = 20;

inline uint16_t angleToTicks(int16_t angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  return SERVO_TICK_MIN +
         ((uint32_t)angle * (SERVO_TICK_MAX - SERVO_TICK_MIN)) / 180;
}

void writeServo(uint8_t i, int16_t angle) {
  pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, angleToTicks(angle)));
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  Serial.println(F("=== ObservePose ==="));
#if defined(ROBOT_LAURENT)
  Serial.println(F("Robot : Laurent (offsets calibres appliques)"));
#elif defined(ROBOT_PHILIPPE)
  Serial.println(F("Robot : Philippe"));
#endif

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  // 1. Pose départ : tous à 90°
  for (uint8_t i = 0; i < 12; i++) writeServo(i, 90);
  delay(1000);

  // 2. Interpolation 2 s vers la pose d'observation
  Serial.println(F("Interpolation vers la pose d'observation..."));
  const uint16_t steps = MOVE_DURATION_MS / STEP_DELAY_MS;
  for (uint16_t s = 1; s <= steps; s++) {
    for (uint8_t i = 0; i < 12; i++) {
      int16_t pos = 90 + ((int32_t)((int16_t)TARGET_DEG[i] - 90) * s) / steps;
      writeServo(i, pos);
    }
    delay(STEP_DELAY_MS);
  }
  // Affirme la position finale
  for (uint8_t i = 0; i < 12; i++) writeServo(i, TARGET_DEG[i]);

  Serial.println(F("Pose atteinte. Angles courants :"));
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(F("  "));
    Serial.print(SERVO_NAMES[i]);
    Serial.print(F(" = "));
    Serial.print(TARGET_DEG[i]);
    Serial.println(F(" deg"));
  }
  Serial.println(F("Note les articulations qui partent dans le mauvais sens."));
}

void loop() {
  // statique
}
