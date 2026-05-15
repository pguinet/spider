/*
 * StandPose - Spider Robot (Arduino Nano)
 *
 * Met le robot en posture debout :
 *   1. Tous les servos à 90° (pose étendue) au démarrage,
 *   2. Interpolation linéaire douce sur 2 s vers la pose debout,
 *   3. Maintien de la pose.
 *
 * ⚠️ Premier essai : poser le robot sur un support pour que les pattes
 *    soient dans le vide, le temps de vérifier que les angles ne forcent pas.
 *    Si une patte part dans le mauvais sens, inverser la valeur du DEG concerné
 *    (par ex. remplacer 135 par 45, ou 60 par 120).
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 *
 * Compilation :
 *   --build-property "build.extra_flags=-DROBOT_LAURENT"
 *   --build-property "build.extra_flags=-DROBOT_PHILIPPE"
 *
 * @dependencies Adafruit PWM Servo Driver Library, Adafruit BusIO
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SpiderConfig.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA9685_ADDR);

// ============================================================
// Angles cible de la pose debout — par robot (polarités différentes)
// ============================================================
// Convention : 90° = neutre.

#if defined(ROBOT_PHILIPPE)
// Philippe : miroir gauche/droite + inversion avant/arrière
const uint8_t FR_COXA_DEG  = 90;
const uint8_t FR_FEMUR_DEG = 60;
const uint8_t FR_TIBIA_DEG = 60;

const uint8_t FL_COXA_DEG  = 90;
const uint8_t FL_FEMUR_DEG = 120;
const uint8_t FL_TIBIA_DEG = 120;

const uint8_t RR_COXA_DEG  = 90;
const uint8_t RR_FEMUR_DEG = 120;
const uint8_t RR_TIBIA_DEG = 120;

const uint8_t RL_COXA_DEG  = 90;
const uint8_t RL_FEMUR_DEG = 60;
const uint8_t RL_TIBIA_DEG = 60;

#elif defined(ROBOT_LAURENT)
// Laurent : miroir gauche/droite, FEMUR/TIBIA validés via ServoPose.
// Pose asymétrique pour stabilité accrue :
//   - Côté gauche en H : COXA à 90° (pattes perpendiculaires au corps)
//   - Côté droit en X : FR vers l'avant (135°), RR vers l'arrière (45°)
const uint8_t FR_COXA_DEG  = 135;
const uint8_t FR_FEMUR_DEG = 60;
const uint8_t FR_TIBIA_DEG = 60;

const uint8_t FL_COXA_DEG  = 90;
const uint8_t FL_FEMUR_DEG = 120;
const uint8_t FL_TIBIA_DEG = 120;

const uint8_t RR_COXA_DEG  = 45;
const uint8_t RR_FEMUR_DEG = 60;
const uint8_t RR_TIBIA_DEG = 60;

const uint8_t RL_COXA_DEG  = 90;
const uint8_t RL_FEMUR_DEG = 120;
const uint8_t RL_TIBIA_DEG = 120;

#else
  #error "Aucun robot defini. Compiler avec -DROBOT_LAURENT ou -DROBOT_PHILIPPE."
#endif

// ============================================================
// Paramètres d'interpolation
// ============================================================
const uint16_t MOVE_DURATION_MS = 2000;
const uint8_t  STEP_DELAY_MS    = 20;
const uint8_t  START_DELAY_S    = 3;   // pause après centrage avant le mouvement

// Tableau cible aligné sur SERVO_CHANNELS (ordre FR, FL, RL, RR × COXA, FEMUR, TIBIA)
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

inline uint16_t angleToTicks(uint16_t angle) {
  // 0° -> SERVO_TICK_MIN, 180° -> SERVO_TICK_MAX
  return SERVO_TICK_MIN +
         ((uint32_t)angle * (SERVO_TICK_MAX - SERVO_TICK_MIN)) / 180;
}

void writeAllAngles(const uint8_t deg[12]) {
  for (uint8_t i = 0; i < 12; i++) {
    pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, angleToTicks(deg[i])));
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

#if defined(ROBOT_LAURENT)
  Serial.println(F("=== StandPose : Laurent (offsets calibres appliques) ==="));
#elif defined(ROBOT_PHILIPPE)
  Serial.println(F("=== StandPose : Philippe ==="));
#endif
  Serial.println(F("ATTENTION : pates suspendues recommande au 1er essai."));

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  // 1. Tous à 90° (pose étendue)
  uint8_t neutral[12];
  for (uint8_t i = 0; i < 12; i++) neutral[i] = 90;
  writeAllAngles(neutral);
  Serial.print(F("Pose etendue (90 deg). Pause "));
  Serial.print(START_DELAY_S);
  Serial.println(F(" s avant la pose debout."));
  delay((uint32_t)START_DELAY_S * 1000);

  // 2. Interpolation linéaire 12 servos en parallèle
  Serial.println(F("Interpolation vers pose debout..."));
  const uint16_t steps = MOVE_DURATION_MS / STEP_DELAY_MS;
  for (uint16_t s = 1; s <= steps; s++) {
    for (uint8_t i = 0; i < 12; i++) {
      int16_t delta = (int16_t)TARGET_DEG[i] - 90;
      uint16_t current = 90 + (delta * (int32_t)s) / steps;
      pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, angleToTicks(current)));
    }
    delay(STEP_DELAY_MS);
  }

  // 3. Maintien (re-affirme la pose finale précise)
  writeAllAngles(TARGET_DEG);

  Serial.println(F("Pose debout atteinte. Maintien."));
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(F("  "));
    Serial.print(SERVO_NAMES[i]);
    Serial.print(F(" = "));
    Serial.print(TARGET_DEG[i]);
    Serial.println(F(" deg"));
  }
}

void loop() {
  // Rien à faire : PCA9685 maintient les consignes PWM.
}
