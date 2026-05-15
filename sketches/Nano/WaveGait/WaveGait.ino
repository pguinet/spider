/*
 * WaveGait - Spider Robot (Arduino Nano)
 *
 * Marche wave gait (statique) : à chaque pas, une seule patte est en l'air,
 * les 3 autres au sol poussent le corps vers l'avant.
 *
 * Cycle de 4 pas dans l'ordre diagonal FR -> RL -> FL -> RR. Pour chaque pas :
 *   1. Lift  : FEMUR + TIBIA replient encore pour décoller le pied.
 *   2. Swing : COXA de la patte courante avance de +COXA_AMP ; pendant ce temps
 *              les 3 autres COXA reculent chacune de COXA_AMP/3 (le corps avance).
 *   3. Drop  : FEMUR + TIBIA reviennent à la pose debout.
 *
 * Polarités héritées de StandPose (Philippe : miroir gauche/droite + inversion
 * avant/arrière sur FEMUR/TIBIA). Le sens d'avancement COXA est à valider
 * visuellement : si le robot recule, inverser coxa_dir.
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
// Paramètres du wave gait — À AJUSTER
// ============================================================
const uint8_t  COXA_AMP    = 30;   // amplitude swing COXA (divisible par 3, pas de drift)
const uint8_t  LIFT_AMP    = 25;   // amplitude levée FEMUR+TIBIA
const uint16_t LIFT_MS     = 200;
const uint16_t SWING_MS    = 400;
const uint16_t DROP_MS     = 200;
const uint8_t  STEP_DELAY  = 20;

// ============================================================
// Configuration par patte
// ============================================================
struct Leg {
  uint8_t coxa_ch, femur_ch, tibia_ch;
  uint8_t coxa_stand;            // angle COXA en pose debout (souvent != 90 pour pose X)
  uint8_t femur_stand, tibia_stand;
  int8_t  lift_dir;              // sens de levée (s'éloigner de 90 dans le bon sens)
  int8_t  coxa_dir;              // sens du COXA pour avancer la patte (à valider)
};

// IMPORTANT : ordre des pattes ALIGNÉ avec SERVO_CHANNELS[] de SpiderConfig.h
// soit FR, FL, RL, RR. Ne pas changer sans aussi modifier SpiderConfig.h.

#if defined(ROBOT_PHILIPPE)
// Philippe : miroir gauche/droite + inversion avant/arrière (FEMUR/TIBIA).
// Pose départ en X à ajuster si besoin.
const Leg LEGS[4] = {
  { FR_COXA, FR_FEMUR, FR_TIBIA, 110,  60,  60, -1, +1 },  // FR — avant-droite
  { FL_COXA, FL_FEMUR, FL_TIBIA,  70, 120, 120, +1, -1 },  // FL — avant-gauche
  { RL_COXA, RL_FEMUR, RL_TIBIA, 110,  60,  60, -1, -1 },  // RL — arrière-gauche
  { RR_COXA, RR_FEMUR, RR_TIBIA,  70, 120, 120, +1, +1 },  // RR — arrière-droite
};
#elif defined(ROBOT_LAURENT)
// Laurent : miroir gauche/droite uniquement, pas d'inversion avant/arrière.
// Pose départ en H symétrique (COXA=90 partout) pour simplifier la marche.
const Leg LEGS[4] = {
  { FR_COXA, FR_FEMUR, FR_TIBIA, 90,  60,  60, -1, +1 },  // FR — côté droit
  { FL_COXA, FL_FEMUR, FL_TIBIA, 90, 120, 120, +1, -1 },  // FL — côté gauche
  { RL_COXA, RL_FEMUR, RL_TIBIA, 90, 120, 120, +1, -1 },  // RL — côté gauche (= FL)
  { RR_COXA, RR_FEMUR, RR_TIBIA, 90,  60,  60, -1, +1 },  // RR — côté droit (= FR)
};
#else
  #error "Aucun robot defini. Compiler avec -DROBOT_LAURENT ou -DROBOT_PHILIPPE."
#endif

// Ordre des swings dans le cycle wave (diagonale alternée)
const uint8_t WAVE_ORDER[4] = { 0, 2, 1, 3 };  // FR -> RL -> FL -> RR
const char* const LEG_NAMES[4] = { "FR", "FL", "RL", "RR" };

// État courant des 12 servos (en degrés)
int16_t cur[12];

// ============================================================
// Utilitaires
// ============================================================
inline uint16_t angleToTicks(int16_t angle) {
  if (angle < 0)   angle = 0;
  if (angle > 180) angle = 180;
  return SERVO_TICK_MIN +
         ((uint32_t)angle * (SERVO_TICK_MAX - SERVO_TICK_MIN)) / 180;
}

inline uint8_t idx(uint8_t leg, uint8_t joint) { return leg * 3 + joint; }

// Interpole en parallèle vers targets[12] sur duration_ms.
// targets[i] < 0  => servo i non modifié.
void interpolate(const int16_t targets[12], uint16_t duration_ms) {
  int16_t starts[12];
  for (uint8_t i = 0; i < 12; i++) starts[i] = cur[i];

  uint16_t steps = duration_ms / STEP_DELAY;
  if (steps == 0) steps = 1;

  for (uint16_t s = 1; s <= steps; s++) {
    for (uint8_t i = 0; i < 12; i++) {
      if (targets[i] < 0) continue;
      int16_t pos = starts[i] +
                    ((int32_t)(targets[i] - starts[i]) * s) / steps;
      pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, angleToTicks(pos)));
      cur[i] = pos;
    }
    delay(STEP_DELAY);
  }
}

// ============================================================
// Pose debout initiale
// ============================================================
void standUp() {
  // Écrit directement la pose debout sans interpolation depuis 90° : si Philippe
  // est déjà debout (cas typique au reboot), il ne bouge pas. Pas de chute brutale.
  for (uint8_t leg = 0; leg < 4; leg++) {
    cur[idx(leg, 0)] = LEGS[leg].coxa_stand;
    cur[idx(leg, 1)] = LEGS[leg].femur_stand;
    cur[idx(leg, 2)] = LEGS[leg].tibia_stand;
  }
  for (uint8_t i = 0; i < 12; i++) {
    pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, angleToTicks(cur[i])));
  }
  delay(800);
}

// ============================================================
// Un pas wave : la patte `leg` swing, les 3 autres reculent
// ============================================================
void waveStep(uint8_t leg) {
  const Leg& L = LEGS[leg];
  int16_t tgt[12];

  // ----- 1. Lift -----
  for (uint8_t i = 0; i < 12; i++) tgt[i] = -1;
  tgt[idx(leg, 1)] = L.femur_stand + L.lift_dir * (int16_t)LIFT_AMP;
  tgt[idx(leg, 2)] = L.tibia_stand + L.lift_dir * (int16_t)LIFT_AMP;
  interpolate(tgt, LIFT_MS);

  // ----- 2. Swing (relatif) -----
  for (uint8_t i = 0; i < 12; i++) tgt[i] = -1;
  // Patte courante : COXA avance de +coxa_dir * COXA_AMP (relatif)
  tgt[idx(leg, 0)] = cur[idx(leg, 0)] + L.coxa_dir * (int16_t)COXA_AMP;
  // 3 autres pattes : COXA recule de coxa_dir/3 (relatif)
  for (uint8_t j = 0; j < 4; j++) {
    if (j == leg) continue;
    tgt[idx(j, 0)] = cur[idx(j, 0)] - LEGS[j].coxa_dir * (int16_t)(COXA_AMP / 3);
  }
  interpolate(tgt, SWING_MS);

  // ----- 3. Drop -----
  for (uint8_t i = 0; i < 12; i++) tgt[i] = -1;
  tgt[idx(leg, 1)] = L.femur_stand;
  tgt[idx(leg, 2)] = L.tibia_stand;
  interpolate(tgt, DROP_MS);
}

// ============================================================
void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  Serial.println(F("=== WaveGait : Philippe ==="));

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  standUp();
  Serial.println(F("Pose debout OK. Wave gait dans 2s..."));
  delay(2000);
}

void loop() {
  for (uint8_t k = 0; k < 4; k++) {
    uint8_t leg = WAVE_ORDER[k];
    Serial.print(F(">>> Swing "));
    Serial.println(LEG_NAMES[leg]);
    waveStep(leg);
  }
}
