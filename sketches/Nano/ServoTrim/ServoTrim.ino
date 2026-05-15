/*
 * ServoTrim - Spider Robot (Arduino Nano)
 *
 * Calibrage interactif des 12 servos à la position neutre (90°).
 * Permet d'appliquer un offset individuel à chaque servo via la série,
 * pour compenser un palonnier monté légèrement de travers.
 *
 * Commandes série (terminer par newline) :
 *   0..11    sélectionne un servo (index logique selon SERVO_CHANNELS)
 *   +        offset += 1 tick (~0.88°) sur le servo courant
 *   -        offset -= 1 tick
 *   +N       offset += N ticks (ex: +5)
 *   -N       offset -= N ticks
 *   r        reset offset du servo courant à 0
 *   ra       reset all offsets
 *   p        affiche la table des 12 offsets
 *   h        aide
 *
 * Quand satisfait, taper `p` et noter les offsets pour les figer en dur.
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
 * Compilation : --build-property "build.extra_flags=-DROBOT_LAURENT"
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

int8_t  offsets[12]  = {0};
uint8_t selected     = 0;

inline uint16_t computeTicks(uint8_t i) {
  int16_t t = (int16_t)SERVO_TICK_CENTER + offsets[i];
  if (t < (int16_t)SERVO_TICK_MIN) t = SERVO_TICK_MIN;
  if (t > (int16_t)SERVO_TICK_MAX) t = SERVO_TICK_MAX;
  return (uint16_t)t;
}

void applyServo(uint8_t i) {
  pwm.setPWM(SERVO_CHANNELS[i], 0, computeTicks(i));
}

void applyAll() {
  for (uint8_t i = 0; i < 12; i++) applyServo(i);
}

void printSelected() {
  Serial.print(F("[selected] "));
  Serial.print(selected);
  Serial.print(F(" = "));
  Serial.print(SERVO_NAMES[selected]);
  Serial.print(F("  offset="));
  Serial.print(offsets[selected]);
  Serial.print(F(" ticks  -> "));
  Serial.print(computeTicks(selected));
  Serial.println(F(" ticks total"));
}

void printAll() {
  Serial.println(F("---- Table des offsets ----"));
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(F("  "));
    if (i < 10) Serial.print(' ');
    Serial.print(i);
    Serial.print(F("  "));
    Serial.print(SERVO_NAMES[i]);
    Serial.print(F("  canal="));
    Serial.print(SERVO_CHANNELS[i]);
    Serial.print(F("  offset="));
    Serial.print(offsets[i]);
    Serial.print(F("  ticks="));
    Serial.println(computeTicks(i));
  }
  Serial.println(F("---------------------------"));
}

void printHelp() {
  Serial.println(F("Commandes :"));
  Serial.println(F("  0..11   selectionne un servo"));
  Serial.println(F("  + / -   ajuste +/- 1 tick"));
  Serial.println(F("  +N/-N   ajuste +/- N ticks"));
  Serial.println(F("  r       reset offset courant"));
  Serial.println(F("  ra      reset all"));
  Serial.println(F("  p       print table"));
  Serial.println(F("  h       aide"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  applyAll();

  Serial.println(F("=== ServoTrim ==="));
#if defined(ROBOT_LAURENT)
  Serial.println(F("Robot : Laurent"));
#elif defined(ROBOT_PHILIPPE)
  Serial.println(F("Robot : Philippe"));
#endif
  Serial.println(F("Tous les servos a 90 deg + offset."));
  printHelp();
  Serial.println();
  printSelected();
}

void processCommand(String line) {
  line.trim();
  if (line.length() == 0) return;

  char c = line.charAt(0);

  if (c == 'h' || c == 'H') {
    printHelp();
    return;
  }
  if (c == 'p' || c == 'P') {
    printAll();
    return;
  }
  if (line == "ra" || line == "RA") {
    for (uint8_t i = 0; i < 12; i++) offsets[i] = 0;
    applyAll();
    Serial.println(F("Tous les offsets remis a 0."));
    return;
  }
  if (c == 'r' || c == 'R') {
    offsets[selected] = 0;
    applyServo(selected);
    Serial.print(F("Reset "));
    Serial.println(SERVO_NAMES[selected]);
    printSelected();
    return;
  }
  if (c == '+' || c == '-') {
    int delta = (line.length() > 1) ? line.substring(1).toInt() : 0;
    if (delta == 0) delta = 1;
    if (c == '-') delta = -delta;
    int16_t newOffset = (int16_t)offsets[selected] + delta;
    if (newOffset < -100) newOffset = -100;
    if (newOffset >  100) newOffset =  100;
    offsets[selected] = (int8_t)newOffset;
    applyServo(selected);
    printSelected();
    return;
  }
  // sinon : tente de parser un index numérique
  int idx = line.toInt();
  if (idx >= 0 && idx <= 11 && (line.charAt(0) >= '0' && line.charAt(0) <= '9')) {
    selected = (uint8_t)idx;
    printSelected();
    return;
  }
  Serial.print(F("Commande inconnue : '"));
  Serial.print(line);
  Serial.println(F("' (tape 'h' pour l'aide)"));
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    processCommand(line);
  }
}
