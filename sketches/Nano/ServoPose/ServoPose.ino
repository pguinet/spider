/*
 * ServoPose - Spider Robot (Arduino Nano)
 *
 * Sketch statique pour observer le sens de rotation des servos.
 * Positionne chaque articulation à un angle précis et l'y maintient,
 * pour observer tranquillement sans oscillation.
 *
 * Applique les offsets de calibrage de SpiderConfig.h
 * (importants pour Laurent, valeurs 0 pour Philippe non calibré).
 *
 * Commandes série (terminer par newline) :
 *   <idx> <angle>   positionne le servo idx (0..11) à l'angle (0..180°)
 *                   ex: "0 60"   -> FR_COXA à 60°
 *                       "5 120"  -> FL_TIBIA à 120°
 *   c               tous les servos à 90° (centre)
 *   p               affiche les angles actuels
 *   h               aide
 *
 * Mapping idx -> articulation (cf. SERVO_CHANNELS dans SpiderConfig.h) :
 *   0 FR_COXA   1 FR_FEMUR   2 FR_TIBIA
 *   3 FL_COXA   4 FL_FEMUR   5 FL_TIBIA
 *   6 RL_COXA   7 RL_FEMUR   8 RL_TIBIA
 *   9 RR_COXA  10 RR_FEMUR  11 RR_TIBIA
 *
 * Board: Arduino Nano (ATmega328P)
 * FQBN: arduino:avr:nano (option :cpu=atmega328old si bootloader ancien)
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

const char* const SERVO_NAMES[12] = {
  "FR_COXA", "FR_FEMUR", "FR_TIBIA",
  "FL_COXA", "FL_FEMUR", "FL_TIBIA",
  "RL_COXA", "RL_FEMUR", "RL_TIBIA",
  "RR_COXA", "RR_FEMUR", "RR_TIBIA"
};

int16_t currentAngle[12];

inline uint16_t angleToTicks(int16_t angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  return SERVO_TICK_MIN +
         ((uint32_t)angle * (SERVO_TICK_MAX - SERVO_TICK_MIN)) / 180;
}

void setServo(uint8_t idx, int16_t angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  currentAngle[idx] = angle;
  pwm.setPWM(SERVO_CHANNELS[idx], 0, calibratedTicks(idx, angleToTicks(angle)));
}

void centerAll() {
  for (uint8_t i = 0; i < 12; i++) setServo(i, 90);
  Serial.println(F("Tous les servos a 90 deg."));
}

void printAll() {
  Serial.println(F("---- Positions courantes ----"));
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(F("  "));
    if (i < 10) Serial.print(' ');
    Serial.print(i);
    Serial.print(F("  "));
    Serial.print(SERVO_NAMES[i]);
    Serial.print(F("  angle="));
    Serial.print(currentAngle[i]);
    Serial.println(F(" deg"));
  }
  Serial.println(F("-----------------------------"));
}

void printHelp() {
  Serial.println(F("Commandes :"));
  Serial.println(F("  <idx> <angle>   positionne servo idx (0-11) a angle (0-180)"));
  Serial.println(F("  c               tous a 90 deg"));
  Serial.println(F("  p               print positions"));
  Serial.println(F("  h               aide"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ_HZ);

  centerAll();

  Serial.println(F("=== ServoPose ==="));
#if defined(ROBOT_LAURENT)
  Serial.println(F("Robot : Laurent (offsets calibres appliques)"));
#elif defined(ROBOT_PHILIPPE)
  Serial.println(F("Robot : Philippe"));
#endif
  printHelp();
}

void processCommand(String line) {
  line.trim();
  if (line.length() == 0) return;

  char c = line.charAt(0);
  if (c == 'h' || c == 'H') { printHelp(); return; }
  if (c == 'p' || c == 'P') { printAll(); return; }
  if (c == 'c' || c == 'C') { centerAll(); return; }

  // Parse "<idx> <angle>"
  int space = line.indexOf(' ');
  if (space < 0) {
    Serial.println(F("Format: <idx> <angle>   (ex: '0 60')"));
    return;
  }
  int idx = line.substring(0, space).toInt();
  int angle = line.substring(space + 1).toInt();
  if (idx < 0 || idx > 11) {
    Serial.println(F("idx hors plage (0-11)"));
    return;
  }
  if (angle < 0 || angle > 180) {
    Serial.println(F("angle hors plage (0-180)"));
    return;
  }
  setServo((uint8_t)idx, (int16_t)angle);
  Serial.print(F("  -> "));
  Serial.print(SERVO_NAMES[idx]);
  Serial.print(F(" = "));
  Serial.print(angle);
  Serial.println(F(" deg"));
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    processCommand(line);
  }
}
