/* CareBot: three preloaded flap bins (2, 6, 2) and two beam grippers.
   Target: DOIT ESP32 DEVKIT V1, Arduino-ESP32 3.x, L298N.
   Drive pair: dual-shaft 12 V, 500 RPM DC geared motors.
   Servos: PCA9685 at I2C address 0x40, channels 0..4.
   Motor stall current and loaded travel speed still require measurement.
   No external servo library required. See ../README.md before wiring/loading.
   Serial 115200: timestamped logger; s = start once, x = stop.
   All distances/timings below require calibration on the actual robot.
*/
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#if !defined(CONFIG_IDF_TARGET_ESP32)
#error "Select a classic ESP32 target such as fESP32 Dev Module."
#endif

// L298N: OUT1/2 = left motor, OUT3/4 = right motor.
// Remove the ENA and ENB jumpers so the ESP32 can control speed with PWM.
constexpr uint8_t MOTOR_DIRECTION_PINS[] = {25, 26, 27, 14}; // IN1..IN4
constexpr uint8_t MOTOR_ENABLE_PINS[] = {33, 17}; // ENA, ENB
constexpr uint8_t TRIG_PIN = 23, ECHO_PIN = 34;
constexpr uint8_t IR_PINS[] = {35, 16}; // left and right rear sensors
constexpr uint8_t START_PIN = 32; // one start button to GND; no stop button
constexpr uint8_t I2C_SDA_PIN = 21, I2C_SCL_PIN = 22;
constexpr uint8_t PCA9685_ADDRESS = 0x40;
constexpr uint8_t SERVO_CHANNELS[] = {0, 1, 2, 3, 4};
constexpr uint8_t PCA9685_MODE1 = 0x00, PCA9685_PRESCALE = 0xfe;
constexpr uint8_t PCA9685_LED0_ON_L = 0x06;
constexpr uint8_t PCA9685_50HZ_PRESCALE = 121;
constexpr float MOTOR_RATED_VOLTAGE = 12.0f;
constexpr unsigned MOTOR_RATED_RPM = 500;
// Servo indices: small A, six-kit bin, small B, right beam, front beam.
constexpr int CLOSED_DEG[] = {15, 15, 15, 35, 35};
constexpr int OPEN_DEG[] = {100, 100, 100, 110, 110};
constexpr uint32_t SERVO_MIN_US = 1000, SERVO_MAX_US = 2000;
constexpr int BLACK_LEVEL = LOW; // change to HIGH if your module is inverted
constexpr bool INVERT_LEFT = false, INVERT_RIGHT = true;
// RPM is not used as wheel speed. Keep these conservative starting values and
// calibrate them with the loaded robot.
constexpr int DRIVE_PWM = 145, SLOW_PWM = 100, TURN_PWM = 125;
constexpr int LEFT_TRIM = 0, RIGHT_TRIM = 0;
constexpr uint32_t TURN_LEFT_MS = 580, TURN_RIGHT_MS = 580;
constexpr float DRIVE_MM_PER_SECOND = 180.0f; // example only; measure at DRIVE_PWM
constexpr float REVERSE_MM_PER_SECOND = 160.0f; // example only; measure separately
constexpr uint32_t SETTLE_MS = 250, RELEASE_MS = 1100;
constexpr uint32_t LEG_TIMEOUT_MS = 25000, PING_INTERVAL_MS = 65;
constexpr uint32_t ECHO_TIMEOUT_US = 25000, LINE_STABLE_MS = 20;
constexpr float MIN_CLEARANCE_MM = 100; // include ALL forward overhangs
constexpr float FIRST_WALL_STOP_MM = 220, LAST_KIT_WALL_STOP_MM = 220;
constexpr float WALL_TOLERANCE_MM = 8;

// Start TOP RIGHT facing LEFT. Two route turns: LEFT -> DOWN -> RIGHT.
// Both route turns are 90-degree left turns; lane corrections add paired turns.
// Count transverse black markers after clearing the starting marker.
// On the supplied map, the first left-side cross-line ENTERS the middle zone;
// it is not its centre. Calibrate the outlet correction for the desired drop.
constexpr unsigned MIDDLE_MARKER_NUMBER = 1;
// At rear-IR detection, an outlet ahead of the IR is already past the line.
// Negative means reverse. Example: outlet 80 mm ahead => approximately -80.
constexpr float MIDDLE_OUTLET_CORRECTION_MM = 0;
constexpr float FIRST_OUTLET_CORRECTION_MM = 0;
constexpr float LAST_OUTLET_CORRECTION_MM = 0;

// Final beam drop: stop once, open both grippers, stay stopped.
// Final leg faces RIGHT/east. Positive lane shift moves UP/north;
// negative moves DOWN/south. Align the right beam to QZ's TOP boundary.
constexpr float BEAM_LANE_SHIFT_MM = 0;
// Example clearance from the RIGHT WALL; calibrate with both beams mounted.
constexpr float BEAM_DROP_WALL_MM = 220;

static_assert(DRIVE_PWM > 0 && DRIVE_PWM <= 255 && SLOW_PWM > 0 &&
              SLOW_PWM <= DRIVE_PWM && TURN_PWM > 0 && TURN_PWM <= 255,
              "Motor PWM values must be within 1..255.");
static_assert(DRIVE_MM_PER_SECOND > 0 && REVERSE_MM_PER_SECOND > 0,
              "Calibrated speeds must be positive.");
static_assert(MOTOR_RATED_VOLTAGE == 12.0f && MOTOR_RATED_RPM == 500,
              "Update the motor ratings before changing its calibration.");
static_assert(SERVO_MIN_US > 0 && SERVO_MAX_US > SERVO_MIN_US &&
              SERVO_MAX_US < 20000, "Invalid servo pulse limits.");
static_assert(MIDDLE_MARKER_NUMBER > 0, "Marker numbers start at one.");
static_assert(TURN_LEFT_MS > 0 && TURN_LEFT_MS <= LEG_TIMEOUT_MS &&
              TURN_RIGHT_MS > 0 && TURN_RIGHT_MS <= LEG_TIMEOUT_MS,
              "Turn duration must fit within a travel leg.");

bool aborted = false, attempted = false;
bool motorPwmAttached[2] = {false, false};
bool startArmed = false, startHeld = false;
bool startupCheckPassed = false;
bool servoDriverReady = false;
uint32_t startPressMs = 0;
bool released[5] = {false, false, false, false, false};
uint32_t lastPingMs = 0;

void logPrefix(const char *level) {
  Serial.print('['); Serial.print(millis()); Serial.print(" ms] [");
  Serial.print(level); Serial.print("] ");
}

void logLine(const char *level, const char *message) {
  logPrefix(level); Serial.println(message);
}

void logValue(const char *level, const char *label, float value,
              const char *unit) {
  logPrefix(level); Serial.print(label); Serial.print(value);
  Serial.print(' '); Serial.println(unit);
}

void stopMotors() {
  for (unsigned i = 0; i < 2; ++i) {
    uint8_t pin = MOTOR_ENABLE_PINS[i];
    if (motorPwmAttached[i]) {
      if (ledcWrite(pin, 0)) continue;
      // If PWM control fails, detach it before forcing the GPIO low.
      ledcDetach(pin);
      motorPwmAttached[i] = false;
      pinMode(pin, OUTPUT);
    }
    digitalWrite(pin, LOW);
  }
  for (uint8_t pin : MOTOR_DIRECTION_PINS) digitalWrite(pin, LOW);
}

bool fail(const char *reason) {
  stopMotors();
  if (!aborted) logLine("ERROR", reason);
  aborted = true;
  return false;
}

bool checkStop() {
  if (aborted) return false;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'x' || c == 'X') return fail("Serial stop.");
  }
  return true;
}

bool waitChecked(uint32_t durationMs) {
  uint32_t start = millis();
  while (millis() - start < durationMs) {
    if (!checkStop()) return false;
    delay(2);
  }
  return checkStop();
}

void setMotor(uint8_t in1, uint8_t in2, uint8_t enable,
              int speed, bool invert) {
  speed = constrain(invert ? -speed : speed, -255, 255);
  // Disable the bridge before changing direction, then apply PWM to ENA/ENB.
  if (!ledcWrite(enable, 0)) { fail("Motor PWM write failed."); return; }
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  if (speed == 0) return;
  digitalWrite(speed > 0 ? in1 : in2, HIGH);
  if (!ledcWrite(enable, abs(speed))) fail("Motor PWM write failed.");
}

void drive(int left, int right) {
  if (aborted) { stopMotors(); return; }
  setMotor(MOTOR_DIRECTION_PINS[0], MOTOR_DIRECTION_PINS[1],
           MOTOR_ENABLE_PINS[0], left, INVERT_LEFT);
  if (aborted) return;
  setMotor(MOTOR_DIRECTION_PINS[2], MOTOR_DIRECTION_PINS[3],
           MOTOR_ENABLE_PINS[1], right, INVERT_RIGHT);
}

void straight(int pwm) {
  if (pwm == 0) { stopMotors(); return; }
  int sign = pwm < 0 ? -1 : 1;
  drive(sign * constrain(abs(pwm) + LEFT_TRIM, 0, 255),
        sign * constrain(abs(pwm) + RIGHT_TRIM, 0, 255));
}

bool pcaWriteRegister(uint8_t registerAddress, uint8_t value) {
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(registerAddress);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool pcaPresent() {
  Wire.beginTransmission(PCA9685_ADDRESS);
  return Wire.endTransmission() == 0;
}

bool initializeServoDriver() {
  servoDriverReady = false;
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!pcaPresent()) return fail("PCA9685 not detected at I2C address 0x40.");
  if (!pcaWriteRegister(PCA9685_MODE1, 0x10) ||
      !pcaWriteRegister(PCA9685_PRESCALE, PCA9685_50HZ_PRESCALE) ||
      !pcaWriteRegister(PCA9685_MODE1, 0x20))
    return fail("PCA9685 50 Hz initialization failed.");
  delay(5);
  if (!pcaWriteRegister(PCA9685_MODE1, 0xa0))
    return fail("PCA9685 restart failed.");
  servoDriverReady = true;
  logLine("INFO", "PCA9685 detected at 0x40 and configured for 50 Hz.");
  return true;
}

bool pcaWriteServo(unsigned channel, uint16_t offTick) {
  if (channel >= 16 || offTick > 4095) return false;
  uint8_t firstRegister = PCA9685_LED0_ON_L + 4 * channel;
  Wire.beginTransmission(PCA9685_ADDRESS);
  Wire.write(firstRegister);
  Wire.write(uint8_t(0));
  Wire.write(uint8_t(0));
  Wire.write(uint8_t(offTick & 0xff));
  Wire.write(uint8_t((offTick >> 8) & 0x0f));
  return Wire.endTransmission() == 0;
}

bool servoAngle(unsigned index, int degrees) {
  if (!checkStop()) return false;
  if (index >= 5 || degrees < 0 || degrees > 180)
    return fail("Invalid servo index or angle.");
  if (!servoDriverReady) return fail("PCA9685 is not initialized.");
  uint32_t pulseUs = SERVO_MIN_US +
    (SERVO_MAX_US - SERVO_MIN_US) * constrain(degrees, 0, 180) / 180;
  uint16_t offTick = uint16_t((pulseUs * 4096UL) / 20000UL);
  if (!pcaWriteServo(SERVO_CHANNELS[index], offTick))
    return fail("PCA9685 servo channel write failed.");
  return true;
}

float rangeMm() {
  uint32_t elapsed = millis() - lastPingMs;
  if (elapsed < PING_INTERVAL_MS && !waitChecked(PING_INTERVAL_MS - elapsed))
    return NAN;
  if (!checkStop()) return NAN;
  lastPingMs = millis();
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  uint32_t us = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT_US);
  // Missing echo is UNKNOWN, never an instruction to continue moving.
  if (us == 0) return NAN;
  float mm = us * 0.343f / 2.0f;
  return mm >= 20 && mm <= 4000 ? mm : NAN;
}

bool settle() { stopMotors(); return waitChecked(SETTLE_MS); }

bool validatePinAssignments() {
  const uint8_t pins[] = {
    MOTOR_DIRECTION_PINS[0], MOTOR_DIRECTION_PINS[1],
    MOTOR_DIRECTION_PINS[2], MOTOR_DIRECTION_PINS[3],
    MOTOR_ENABLE_PINS[0], MOTOR_ENABLE_PINS[1], TRIG_PIN, ECHO_PIN,
    IR_PINS[0], IR_PINS[1], START_PIN, I2C_SDA_PIN, I2C_SCL_PIN
  };
  constexpr unsigned pinCount = sizeof(pins) / sizeof(pins[0]);
  for (unsigned i = 0; i < pinCount; ++i)
    for (unsigned j = i + 1; j < pinCount; ++j)
      if (pins[i] == pins[j]) return fail("Duplicate GPIO assignment.");
  logLine("INFO", "GPIO assignment check passed.");
  return true;
}

bool startupSelfCheck() {
  logLine("INFO", "Starting hardware self-check.");
  stopMotors();
  float distance = rangeMm();
  if (!isfinite(distance))
    return fail("Ultrasonic self-check failed: no valid echo from 20 to 4000 mm.");
  logValue("INFO", "Ultrasonic distance: ", distance, "mm");
  if (!pcaPresent()) return fail("PCA9685 stopped responding at address 0x40.");
  logLine("INFO", "PCA9685 I2C response confirmed.");
  for (unsigned i = 0; i < 2; ++i) {
    logPrefix("INFO"); Serial.print(i == 0 ? "Left IR: " : "Right IR: ");
    Serial.println(digitalRead(IR_PINS[i]) == BLACK_LEVEL ? "BLACK" : "CLEAR");
  }
  if (digitalRead(START_PIN) == LOW)
    logLine("WARN", "START is held. Release it before trying to start.");
  else
    logLine("INFO", "START input is released.");
  logLine("INFO", "L298N PWM outputs initialized and motors held off.");
  logLine("WARN", "No motor feedback sensor: rotation and driver current are not verified.");
  logLine("WARN", "PCA9685 has no position feedback: servo movement is not verified.");
  startupCheckPassed = true;
  logLine("INFO", "Startup self-check passed.");
  return true;
}

// Move toward a wall OR reverse away from it, selected from the initial reading.
// Confirm three target readings while stopped; bound the entire maneuver.
bool wallDistance(float targetMm) {
  stopMotors();
  logValue("INFO", "Wall target: ", targetMm, "mm");
  if (!isfinite(targetMm) || targetMm < MIN_CLEARANCE_MM + WALL_TOLERANCE_MM ||
      targetMm > 4000 - WALL_TOLERANCE_MM)
    return fail("Invalid wall target.");
  float initial = rangeMm();
  if (!isfinite(initial)) return fail("No wall reading at start of move.");
  logValue("INFO", "Initial wall distance: ", initial, "mm");
  if (initial < MIN_CLEARANCE_MM) return fail("Insufficient front clearance.");
  bool forward = initial > targetMm;
  unsigned confirmed = 0, invalid = 0;
  uint32_t start = millis();
  while (millis() - start < LEG_TIMEOUT_MS) {
    if (!checkStop()) return false;
    float distance = rangeMm();
    if (!isfinite(distance)) {
      stopMotors(); confirmed = 0;
      logLine("WARN", "Invalid ultrasonic reading during wall move.");
      if (++invalid >= 3) return fail("Ultrasonic lost the wall.");
      continue;
    }
    invalid = 0;
    if (distance < MIN_CLEARANCE_MM) return fail("Front clearance limit.");
    bool reached = fabsf(distance - targetMm) <= WALL_TOLERANCE_MM;
    if (reached) {
      stopMotors();
      if (++confirmed >= 3) {
        logValue("INFO", "Wall target confirmed at: ", distance, "mm");
        return settle();
      }
    } else {
      confirmed = 0;
      // Do not release a load merely because we crossed the target coordinate.
      if ((forward && distance < targetMm - WALL_TOLERANCE_MM) ||
          (!forward && distance > targetMm + WALL_TOLERANCE_MM))
        return fail("Wall target overshot. Reduce speed or recalibrate.");
      int pwm = fabsf(distance - targetMm) < 150 ? SLOW_PWM : DRIVE_PWM;
      straight(forward ? pwm : -pwm);
    }
  }
  return fail("Wall move timed out.");
}

bool moveMm(float mm) {
  stopMotors();
  if (!checkStop()) return false;
  if (!isfinite(mm)) return fail("Invalid timed distance.");
  if (fabsf(mm) < 1) return settle();
  float durationMs = 1000 * fabsf(mm) /
    (mm > 0 ? DRIVE_MM_PER_SECOND : REVERSE_MM_PER_SECOND);
  if (!isfinite(durationMs) || durationMs > LEG_TIMEOUT_MS)
    return fail("Timed move exceeds limit.");
  uint32_t duration = uint32_t(ceilf(durationMs));
  logValue("INFO", "Timed move distance: ", mm, "mm");
  logPrefix("INFO"); Serial.print("Timed move duration: ");
  Serial.print(duration); Serial.println(" ms");
  // Validate the forward path BEFORE starting the motion timer.
  if (mm > 0) {
    float distance = rangeMm();
    if (!isfinite(distance)) return fail("No echo before forward offset.");
    if (distance <= MIN_CLEARANCE_MM) return fail("Obstacle before offset.");
  }
  straight(mm > 0 ? DRIVE_PWM : -DRIVE_PWM);
  if (!checkStop()) return false;
  uint32_t start = millis();
  // Reverse has no obstacle sensor. Use only on a verified clear return path.
  while (millis() - start < duration) {
    if (!checkStop()) return false;
    uint32_t elapsed = millis() - start;
    if (elapsed >= duration) break;
    uint32_t remaining = duration - elapsed;
    // Never start a blocking echo measurement too near the stop deadline.
    if (mm > 0 && millis() - lastPingMs >= PING_INTERVAL_MS &&
        remaining > (ECHO_TIMEOUT_US + 999) / 1000 + 2) {
      float distance = rangeMm();
      if (!isfinite(distance)) return fail("No echo during forward offset.");
      if (distance < MIN_CLEARANCE_MM) return fail("Obstacle during offset.");
    }
    if (millis() - start < duration) delay(1);
  }
  return settle();
}

bool turn90(bool left) {
  if (!checkStop()) return false;
  logPrefix("INFO"); Serial.print("Turn 90 degrees ");
  Serial.print(left ? "left" : "right"); Serial.print(" for ");
  Serial.print(left ? TURN_LEFT_MS : TURN_RIGHT_MS); Serial.println(" ms");
  // Timing cannot measure angle or side clearance. Check the loaded swept area.
  drive(left ? -TURN_PWM : TURN_PWM, left ? TURN_PWM : -TURN_PWM);
  if (!waitChecked(left ? TURN_LEFT_MS : TURN_RIGHT_MS)) return false;
  return settle();
}

bool bothIrSensorsBlack() {
  return digitalRead(IR_PINS[0]) == BLACK_LEVEL &&
         digitalRead(IR_PINS[1]) == BLACK_LEVEL;
}

bool middleMarker() {
  stopMotors();
  float initial = rangeMm();
  if (!isfinite(initial)) return fail("No echo before seeking marker.");
  if (initial <= LAST_KIT_WALL_STOP_MM) return fail("Wall before middle marker.");
  logLine("INFO", "Left middle: seek entry marker for six-kit delivery.");
  uint32_t start = millis(), changeMs = start;
  bool raw = bothIrSensorsBlack(), stable = raw;
  bool armed = false;
  unsigned count = 0;
  while (millis() - start < LEG_TIMEOUT_MS) {
    if (!checkStop()) return false;
    if (millis() - lastPingMs >= PING_INTERVAL_MS) {
      float distance = rangeMm();
      if (!isfinite(distance)) return fail("No echo while seeking line.");
      if (distance <= LAST_KIT_WALL_STOP_MM) return fail("Wall before middle marker.");
    }
    bool now = bothIrSensorsBlack();
    if (now != raw) { raw = now; changeMs = millis(); }
    if (millis() - changeMs >= LINE_STABLE_MS) {
      if (!raw) armed = true; // must see clear floor before counting black
      if (raw && !stable && armed) {
        armed = false;
        ++count;
        logPrefix("INFO"); Serial.print("Black marker count: ");
        Serial.println(count);
        if (count == MIDDLE_MARKER_NUMBER) return settle();
      }
      stable = raw;
    }
    straight(SLOW_PWM); // slower crossing gives the rear sensor time to sample
    delay(2);
  }
  return fail("Middle marker not detected before timeout.");
}

bool releaseLoad(unsigned index) {
  if (index >= 5 || released[index]) return fail("Invalid or repeated release.");
  if (!settle()) return false;
  logPrefix("INFO"); Serial.print("Release servo "); Serial.println(index + 1);
  if (!servoAngle(index, OPEN_DEG[index])) return false;
  released[index] = true;
  if (!waitChecked(RELEASE_MS)) return false;
  // Leave the flap/gripper open. Close only on reset before reloading.
  return waitChecked(SETTLE_MS);
}

bool releaseBeams() {
  if (released[3] || released[4]) return fail("Repeated beam release.");
  if (!settle()) return false;
  logLine("INFO", "Opening both beam grippers.");
  if (!servoAngle(3, OPEN_DEG[3])) return false;
  released[3] = true;
  if (!servoAngle(4, OPEN_DEG[4])) return false;
  released[4] = true;
  return waitChecked(RELEASE_MS); // no further drive or closing commands
}

bool runMission() {
  logLine("INFO", "Mission started.");
  if (!(isfinite(BEAM_LANE_SHIFT_MM) && isfinite(BEAM_DROP_WALL_MM) &&
        BEAM_DROP_WALL_MM >= MIN_CLEARANCE_MM + WALL_TOLERANCE_MM &&
        BEAM_DROP_WALL_MM <= 4000 - WALL_TOLERANCE_MM))
    return fail("Invalid beam drop settings.");
  logLine("INFO", "Top right -> top left wall; turn down; drop two kits.");
  if (!wallDistance(FIRST_WALL_STOP_MM) || !turn90(true) ||
      !moveMm(FIRST_OUTLET_CORRECTION_MM) || !releaseLoad(0)) return false;
  if (!middleMarker() || !moveMm(MIDDLE_OUTLET_CORRECTION_MM) ||
      !releaseLoad(1)) return false;
  logLine("INFO", "Bottom left wall: drop last two kits.");
  if (!wallDistance(LAST_KIT_WALL_STOP_MM) ||
      !moveMm(LAST_OUTLET_CORRECTION_MM) || !releaseLoad(2)) return false;
  logLine("INFO", "Turn left to face right; align lane above bottom-right quarantine.");
  if (!turn90(true)) return false;
  if (fabsf(BEAM_LANE_SHIFT_MM) >= 1) {
    bool north = BEAM_LANE_SHIFT_MM > 0;
    if (!turn90(north) || !moveMm(fabsf(BEAM_LANE_SHIFT_MM)) ||
        !turn90(!north)) return false;
  }
  if (!wallDistance(BEAM_DROP_WALL_MM)) return false;
  logLine("INFO", "Open both beam grippers; drop beams and stop.");
  if (!releaseBeams()) return false;
  logLine("INFO", "Mission complete. Reset and reload before another run.");
  return true;
}

void setup() {
  Serial.begin(115200);
  startupCheckPassed = false;
  delay(50);
  logLine("INFO", "CareBot booting.");
  for (uint8_t pin : MOTOR_DIRECTION_PINS) {
    pinMode(pin, OUTPUT); digitalWrite(pin, LOW);
  }
  for (uint8_t pin : MOTOR_ENABLE_PINS) {
    pinMode(pin, OUTPUT); digitalWrite(pin, LOW);
  }
  pinMode(TRIG_PIN, OUTPUT); digitalWrite(TRIG_PIN, LOW);
  pinMode(ECHO_PIN, INPUT);
  for (uint8_t pin : IR_PINS) pinMode(pin, INPUT);
  pinMode(START_PIN, INPUT_PULLUP);
  if (!validatePinAssignments()) return;
  // ENA and ENB use the ESP32 LEDC channels 0 and 1.
  for (unsigned i = 0; i < 2; ++i) {
    if (!ledcAttachChannel(MOTOR_ENABLE_PINS[i], 20000, 8, i)) {
      fail("Motor PWM allocation failed."); return;
    }
    motorPwmAttached[i] = true;
    if (!ledcWrite(MOTOR_ENABLE_PINS[i], 0)) {
      fail("Motor PWM initialization failed."); return;
    }
  }
  logLine("INFO", "L298N PWM channels initialized.");
  if (!initializeServoDriver()) return;
  for (unsigned i = 0; i < 5; ++i) {
    if (!servoAngle(i, CLOSED_DEG[i])) return;
  }
  logLine("INFO", "PCA9685 channels 0 through 4 commanded closed.");
  if (!waitChecked(800)) return;
  logPrefix("INFO"); Serial.print("Motor: "); Serial.print(MOTOR_RATED_VOLTAGE);
  Serial.print(" V, "); Serial.print(MOTOR_RATED_RPM);
  Serial.println(" RPM through L298N.");
  if (!startupSelfCheck()) return;
  logLine("INFO", "Ready. Load bins 2/6/2. Press START or send s. x stops.");
}

void loop() {
  stopMotors();
  if (aborted || attempted || !startupCheckPassed) { delay(10); return; }
  bool start = false;
  bool pressed = digitalRead(START_PIN) == LOW;
  if (!pressed) { startArmed = true; startHeld = false; }
  else if (startArmed) {
    if (!startHeld) { startHeld = true; startPressMs = millis(); }
    start = millis() - startPressMs >= 40;
  }
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'x' || c == 'X') { fail("Serial stop."); return; }
    if (c == 's' || c == 'S') {
      logLine("INFO", "Serial start command received.");
      start = true;
    }
  }
  if (start && checkStop()) {
    logLine("INFO", "Start accepted.");
    attempted = true;
    if (!runMission() && !aborted) fail("Mission stopped without a reported fault.");
    stopMotors();
  }
  delay(5);
}

