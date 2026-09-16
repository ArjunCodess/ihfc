/* CareBot: three preloaded flap bins (2, 6, 2) and two beam grippers.
   Target: common 30-pin ESP32-WROOM-32 DevKit, Arduino-ESP32 3.x, L298N.
   Drive pair: dual-shaft 12 V, 500 RPM DC geared motors.
   Servos: PCA9685 at I2C address 0x40, channels 0..4.
   Motor stall current and loaded travel speed still require measurement.
   Requires Adafruit PWM Servo Driver Library and Adafruit BusIO.
   The mission starts automatically after power-up checks. Serial x stops it.
   All distances/timings below require calibration on the actual robot.
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#if !defined(CONFIG_IDF_TARGET_ESP32)
#error "Select a classic 30-pin ESP32-WROOM-32 Dev Module."
#endif

// L298N: OUT1/2 = left motor, OUT3/4 = right motor.
// Remove the ENA and ENB jumpers so the ESP32 can control speed with PWM.
constexpr uint8_t MOTOR_DIRECTION_PINS[] = {25, 26, 27, 14}; // IN1..IN4
constexpr uint8_t MOTOR_ENABLE_PINS[] = {33, 17}; // ENA, ENB
constexpr uint8_t TRIG_PIN = 23, ECHO_PIN = 34;
constexpr uint8_t IR_PINS[] = {35, 16}; // left and right front floor sensors
constexpr uint8_t I2C_SDA_PIN = 21, I2C_SCL_PIN = 22;
constexpr uint8_t PCA9685_ADDRESS = 0x40;
constexpr uint8_t SERVO_CHANNELS[] = {0, 1, 2, 3, 4};
constexpr uint8_t PCA9685_50HZ_PRESCALE = 121;
Adafruit_PWMServoDriver pwm(PCA9685_ADDRESS);
constexpr uint16_t SERVO_TEST_LOW_TICKS = 225;
constexpr uint16_t SERVO_TEST_CENTER_TICKS = 375;
constexpr float MOTOR_RATED_VOLTAGE = 12.0f;
constexpr unsigned MOTOR_RATED_RPM = 500;
// Servo indices: 2-bin A, 6-bin, 2-bin B, first beam, second beam.
// The two MG995 2-bin servos move below rest; the 6-bin servo moves above it.
constexpr int REST_DEG[] = {90, 90, 90, 35, 35};
constexpr int RELEASE_DEG[] = {20, 160, 20, 110, 110};
constexpr uint32_t SERVO_MIN_US = 1000, SERVO_MAX_US = 2000;
constexpr int BLACK_LEVEL = LOW; // change to HIGH if your module is inverted
constexpr bool INVERT_LEFT = false, INVERT_RIGHT = true;
// RPM is not used as wheel speed. Keep these conservative starting values and
// calibrate them with the loaded robot.
constexpr int DRIVE_PWM = 102, SLOW_PWM = 102, TURN_PWM = 125;
constexpr int LEFT_TRIM = 0, RIGHT_TRIM = 0;
constexpr uint32_t TURN_LEFT_MS = 580, TURN_RIGHT_MS = 580;
constexpr float DRIVE_MM_PER_SECOND = 180.0f; // example only; measure at DRIVE_PWM
constexpr float REVERSE_MM_PER_SECOND = 160.0f; // example only; measure separately
constexpr uint32_t SETTLE_MS = 250, RELEASE_MS = 1100;
constexpr uint32_t LEG_TIMEOUT_MS = 25000, PING_INTERVAL_MS = 65;
constexpr uint32_t ECHO_TIMEOUT_US = 25000, LINE_STABLE_MS = 20;
constexpr float WALL_STOP_MM = 200; // one ultrasonic stop threshold for all forward travel

// Start in the marked starting position and use 90-degree left turns.
// Count the transverse black marker after clearing the starting marker.
// On the supplied map, the first left-side cross-line ENTERS the middle zone;
// it is not its centre. Calibrate the outlet correction for the desired drop.
constexpr unsigned MIDDLE_MARKER_NUMBER = 1;
// At front-IR detection, an outlet behind the IR has not reached the line yet.
// Negative means reverse. Example: outlet 80 mm ahead => approximately -80.
constexpr float MIDDLE_OUTLET_CORRECTION_MM = 0;
constexpr float FIRST_OUTLET_CORRECTION_MM = 0;
constexpr float LAST_OUTLET_CORRECTION_MM = 0;

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
bool startupCheckPassed = false;
bool servoDriverReady = false;
bool released[5] = {false, false, false, false, false};
uint32_t lastPingMs = 0;
int lastLeftMotorCommand = 0, lastRightMotorCommand = 0;

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
  if (lastLeftMotorCommand != 0 || lastRightMotorCommand != 0)
    logLine("MOTOR", "left=0 right=0 STOP");
  lastLeftMotorCommand = 0;
  lastRightMotorCommand = 0;
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
  if (!aborted &&
      (left != lastLeftMotorCommand || right != lastRightMotorCommand)) {
    lastLeftMotorCommand = left;
    lastRightMotorCommand = right;
    logPrefix("MOTOR"); Serial.print("left="); Serial.print(left);
    Serial.print(" right="); Serial.println(right);
  }
}

void straight(int pwm) {
  if (pwm == 0) { stopMotors(); return; }
  int sign = pwm < 0 ? -1 : 1;
  drive(sign * constrain(abs(pwm) + LEFT_TRIM, 0, 255),
        sign * constrain(abs(pwm) + RIGHT_TRIM, 0, 255));
}

bool pcaPresent() {
  Wire.beginTransmission(PCA9685_ADDRESS);
  return Wire.endTransmission() == 0;
}

bool initializeServoDriver() {
  servoDriverReady = false;
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!pcaPresent()) return fail("PCA9685 not detected at I2C address 0x40.");
  if (!pwm.begin()) return fail("PCA9685 library initialization failed.");
  pwm.setPWMFreq(50);
  if (!pcaPresent() || pwm.readPrescale() != PCA9685_50HZ_PRESCALE)
    return fail("PCA9685 50 Hz prescale was not confirmed.");
  servoDriverReady = true;
  logLine("INFO", "PCA9685 detected at 0x40; Adafruit driver set to 50 Hz.");
  return true;
}

bool pcaWriteServo(unsigned channel, uint16_t offTick) {
  if (channel >= 16 || offTick > 4095) return false;
  return pwm.setPWM(channel, 0, offTick) == 0;
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
  logPrefix("SERVO"); Serial.print("channel=");
  Serial.print(SERVO_CHANNELS[index]); Serial.print(" target=");
  Serial.print(degrees); Serial.println(" deg");
  return true;
}

// Serial 0..9 or A..F tests one PCA9685 socket while the drive motors stay off.
// Use this with empty mechanisms; it does not start the mission.
bool testServoChannel(unsigned channel) {
  if (channel >= 16 || !servoDriverReady) return false;
  stopMotors();
  logPrefix("INFO"); Serial.print("Servo test channel "); Serial.println(channel);
  if (!pcaWriteServo(channel, SERVO_TEST_CENTER_TICKS) ||
      !waitChecked(600) ||
      !pcaWriteServo(channel, SERVO_TEST_LOW_TICKS) ||
      !waitChecked(600) ||
      !pcaWriteServo(channel, SERVO_TEST_CENTER_TICKS) ||
      !waitChecked(600))
    return aborted ? false : fail("Servo test I2C write failed.");
  // Restore the mission's rest position if this is a configured channel.
  for (unsigned i = 0; i < 5; ++i)
    if (SERVO_CHANNELS[i] == channel && !servoAngle(i, REST_DEG[i])) return false;
  stopMotors();
  logLine("INFO", "Servo test finished; motors remained off.");
  return true;
}

int servoTestChannelFromCommand(char command) {
  if (command >= '0' && command <= '9') return command - '0';
  if (command >= 'A' && command <= 'F') return command - 'A' + 10;
  if (command >= 'a' && command <= 'f') return command - 'a' + 10;
  return -1;
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
  if (us == 0) {
    logLine("ULTRASONIC", "no echo");
    return NAN;
  }
  float mm = us * 0.343f / 2.0f;
  logValue("ULTRASONIC", "distance: ", mm, "mm");
  return mm >= 20 && mm <= 4000 ? mm : NAN;
}

bool readFrontIrSensors(bool logReading = true) {
  static int lastState = -1;
  bool leftBlack = digitalRead(IR_PINS[0]) == BLACK_LEVEL;
  bool rightBlack = digitalRead(IR_PINS[1]) == BLACK_LEVEL;
  int state = (leftBlack ? 2 : 0) | (rightBlack ? 1 : 0);
  if (logReading && state != lastState) {
    lastState = state;
    logPrefix("IR"); Serial.print("front-left=");
    Serial.print(leftBlack ? "BLACK" : "CLEAR");
    Serial.print(" front-right=");
    Serial.println(rightBlack ? "BLACK" : "CLEAR");
  }
  return leftBlack && rightBlack;
}

bool settle() { stopMotors(); return waitChecked(SETTLE_MS); }

bool validatePinAssignments() {
  const uint8_t pins[] = {
    MOTOR_DIRECTION_PINS[0], MOTOR_DIRECTION_PINS[1],
    MOTOR_DIRECTION_PINS[2], MOTOR_DIRECTION_PINS[3],
    MOTOR_ENABLE_PINS[0], MOTOR_ENABLE_PINS[1], TRIG_PIN, ECHO_PIN,
    IR_PINS[0], IR_PINS[1], I2C_SDA_PIN, I2C_SCL_PIN
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
  readFrontIrSensors();
  logLine("INFO", "L298N PWM outputs initialized and motors held off.");
  logLine("WARN", "No motor feedback sensor: rotation and driver current are not verified.");
  logLine("WARN", "PCA9685 has no position feedback: servo movement is not verified.");
  startupCheckPassed = true;
  logLine("INFO", "Startup self-check passed.");
  return true;
}

// Drive forward continuously until the first valid reading at or below 20 cm.
bool approachWall() {
  stopMotors();
  logValue("INFO", "Wall stop threshold: ", WALL_STOP_MM, "mm");
  float initial = rangeMm();
  if (!isfinite(initial)) return fail("No wall reading at start of move.");
  logValue("INFO", "Initial wall distance: ", initial, "mm");
  if (initial <= WALL_STOP_MM) {
    logValue("INFO", "Wall stop at: ", initial, "mm");
    return settle();
  }
  uint32_t start = millis();
  straight(SLOW_PWM);
  if (!checkStop()) return false;
  while (millis() - start < LEG_TIMEOUT_MS) {
    float distance = rangeMm();
    if (!isfinite(distance)) return fail("Ultrasonic lost the wall.");
    if (distance <= WALL_STOP_MM) {
      stopMotors();
      logValue("INFO", "Wall stop at: ", distance, "mm");
      return settle();
    }
  }
  return fail("Wall approach timed out.");
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
    if (distance <= WALL_STOP_MM) return fail("Wall before forward offset.");
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
      if (distance <= WALL_STOP_MM) return fail("Wall during forward offset.");
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
  return readFrontIrSensors();
}

bool middleMarker() {
  stopMotors();
  float initial = rangeMm();
  if (!isfinite(initial)) return fail("No echo before seeking marker.");
  if (initial <= WALL_STOP_MM) return fail("Wall before middle marker.");
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
      if (distance <= WALL_STOP_MM) return fail("Wall before middle marker.");
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
    straight(SLOW_PWM); // slower crossing gives the front sensors time to sample
    delay(2);
  }
  return fail("Middle marker not detected before timeout.");
}

bool releaseLoad(unsigned index) {
  if (index >= 5 || released[index]) return fail("Invalid or repeated release.");
  if (!settle()) return false;
  logPrefix("INFO"); Serial.print("Release servo "); Serial.println(index + 1);
  if (!servoAngle(index, RELEASE_DEG[index])) return false;
  released[index] = true;
  if (!waitChecked(RELEASE_MS)) return false;
  // Leave the flap/gripper open. Close only on reset before reloading.
  return waitChecked(SETTLE_MS);
}

bool runMission() {
  logLine("INFO", "Mission started.");
  logLine("INFO", "Move to 20 cm wall stop, turn left, release two kits.");
  if (!approachWall() || !turn90(true) ||
      !moveMm(FIRST_OUTLET_CORRECTION_MM) || !releaseLoad(0)) return false;
  logLine("INFO", "Seek black tape with both front IR sensors; release six kits.");
  if (!middleMarker() || !moveMm(MIDDLE_OUTLET_CORRECTION_MM) ||
      !releaseLoad(1)) return false;
  logLine("INFO", "Move to the next 20 cm wall stop; release two kits.");
  if (!approachWall() ||
      !moveMm(LAST_OUTLET_CORRECTION_MM) || !releaseLoad(2)) return false;
  logLine("INFO", "Turn left and move to the next 20 cm wall stop.");
  if (!turn90(true) || !approachWall()) return false;
  logLine("INFO", "Turn left and release the first beam.");
  if (!turn90(true) || !releaseLoad(3)) return false;
  logLine("INFO", "Turn left and release the second beam.");
  if (!turn90(true) || !releaseLoad(4)) return false;
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
    if (!servoAngle(i, REST_DEG[i])) return;
  }
  logLine("INFO", "PCA9685 channels 0 through 4 commanded to rest positions.");
  if (!waitChecked(800)) return;
  logPrefix("INFO"); Serial.print("Motor: "); Serial.print(MOTOR_RATED_VOLTAGE);
  Serial.print(" V, "); Serial.print(MOTOR_RATED_RPM);
  Serial.println(" RPM through L298N.");
  if (!startupSelfCheck()) return;
  logLine("INFO", "Initialization complete. Starting mission automatically.");
  attempted = true;
  if (!runMission() && !aborted) fail("Mission stopped without a reported fault.");
  stopMotors();
}

void loop() {
  stopMotors();
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'x' || c == 'X') { fail("Serial stop."); return; }
  }
  delay(10);
}

