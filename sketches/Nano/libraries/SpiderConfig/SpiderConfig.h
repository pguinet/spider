/*
 * SpiderConfig.h - Mapping des servos sur le PCA9685
 *
 * Convention articulations (terminologie araignée) :
 *   COXA  = hanche (rotation horizontale, axe Z)
 *   FEMUR = cuisse (élévation, axe horizontal)
 *   TIBIA = genou  (flexion, axe horizontal)
 *
 * Convention pattes (vue de dessus, robot regardant vers l'avant) :
 *   FR = Front-Right    FL = Front-Left
 *   RR = Rear-Right     RL = Rear-Left
 *
 * Sélection du robot à la compilation (un seul des deux) :
 *   --build-property build.extra_flags=-DROBOT_LAURENT
 *   --build-property build.extra_flags=-DROBOT_PHILIPPE
 */

#pragma once
#include <stdint.h>

// Adresse I2C du PCA9685 (identique sur les deux robots)
constexpr uint8_t PCA9685_ADDR = 0x40;

// PWM ticks à 50 Hz (4096 ticks / période 20 ms)
//   ~1.0 ms ≈ 205  -> 0°
//   ~1.5 ms ≈ 307  -> 90°  (neutre)
//   ~2.0 ms ≈ 410  -> 180°
constexpr uint16_t SERVO_TICK_MIN    = 205;
constexpr uint16_t SERVO_TICK_CENTER = 307;
constexpr uint16_t SERVO_TICK_MAX    = 410;
constexpr uint16_t SERVO_FREQ_HZ     = 50;

// =============================================================
// Mapping canal PCA9685 -> articulation (par robot)
// =============================================================

#if defined(ROBOT_PHILIPPE)

// Philippe — câblage confirmé 2026-05-15
//   Ordre des pattes : FR, RR, FL, RL
//   Canaux utilisés  : 0..11 (12..15 libres)
constexpr uint8_t FR_COXA  =  0;
constexpr uint8_t FR_FEMUR =  1;
constexpr uint8_t FR_TIBIA =  2;
constexpr uint8_t RR_COXA  =  3;
constexpr uint8_t RR_FEMUR =  4;
constexpr uint8_t RR_TIBIA =  5;
constexpr uint8_t FL_COXA  =  6;
constexpr uint8_t FL_FEMUR =  7;
constexpr uint8_t FL_TIBIA =  8;
constexpr uint8_t RL_COXA  =  9;
constexpr uint8_t RL_FEMUR = 10;
constexpr uint8_t RL_TIBIA = 11;

#elif defined(ROBOT_LAURENT)

// Laurent — câblage confirmé 2026-05-15
//   Ordre des pattes : FR, FL, RL, RR
//   Canaux utilisés  : 0,1,2 | 4,5,6 | 8,9,10 | 12,13,14 (3,7,11,15 libres)
constexpr uint8_t FR_COXA  =  0;
constexpr uint8_t FR_FEMUR =  1;
constexpr uint8_t FR_TIBIA =  2;
constexpr uint8_t FL_COXA  =  4;
constexpr uint8_t FL_FEMUR =  5;
constexpr uint8_t FL_TIBIA =  6;
constexpr uint8_t RL_COXA  =  8;
constexpr uint8_t RL_FEMUR =  9;
constexpr uint8_t RL_TIBIA = 10;
constexpr uint8_t RR_COXA  = 12;
constexpr uint8_t RR_FEMUR = 13;
constexpr uint8_t RR_TIBIA = 14;

#else
  #error "Aucun robot defini. Compiler avec -DROBOT_LAURENT ou -DROBOT_PHILIPPE."
#endif

// Liste ordonnée des 12 canaux servo (utile pour les boucles).
// L'ordre logique est toujours FR, FL, RL, RR puis COXA/FEMUR/TIBIA, peu importe
// quel canal physique du PCA9685 c'est sur chaque robot.
constexpr uint8_t SERVO_CHANNELS[12] = {
  FR_COXA, FR_FEMUR, FR_TIBIA,
  FL_COXA, FL_FEMUR, FL_TIBIA,
  RL_COXA, RL_FEMUR, RL_TIBIA,
  RR_COXA, RR_FEMUR, RR_TIBIA
};

// =============================================================
// Offsets de calibrage par servo (en ticks PCA9685, 1 tick ≈ 0.88°)
// =============================================================
// Compense les imperfections mécaniques (palonniers légèrement de travers).
// Aligné sur SERVO_CHANNELS. Mesuré avec le sketch ServoTrim.
// Convention : ticks effectifs = ticks_logiques + SERVO_TICK_OFFSETS[idx]

#if defined(ROBOT_LAURENT)
// Mesuré 2026-05-15 via ServoTrim
constexpr int8_t SERVO_TICK_OFFSETS[12] = {
    0,   //  0 : FR_COXA
    4,   //  1 : FR_FEMUR
    0,   //  2 : FR_TIBIA
  -14,   //  3 : FL_COXA
   15,   //  4 : FL_FEMUR
   15,   //  5 : FL_TIBIA
    0,   //  6 : RL_COXA
    0,   //  7 : RL_FEMUR
  -10,   //  8 : RL_TIBIA
    0,   //  9 : RR_COXA
  -25,   // 10 : RR_FEMUR
  -10    // 11 : RR_TIBIA
};
#else  // ROBOT_PHILIPPE — non calibré
constexpr int8_t SERVO_TICK_OFFSETS[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
#endif

// Helper : applique l'offset de calibrage à un nombre de ticks.
// À utiliser au moment d'écrire au PCA9685 :
//   pwm.setPWM(SERVO_CHANNELS[i], 0, calibratedTicks(i, base_ticks));
inline uint16_t calibratedTicks(uint8_t idx, int16_t base_ticks) {
  int16_t t = base_ticks + (int16_t)SERVO_TICK_OFFSETS[idx];
  if (t < (int16_t)SERVO_TICK_MIN) t = SERVO_TICK_MIN;
  if (t > (int16_t)SERVO_TICK_MAX) t = SERVO_TICK_MAX;
  return (uint16_t)t;
}
