// Simple CareBot route for the ESP32 and wiring in this folder's WIRING.md.
// Press the START button on GPIO32 to run once. Send x over Serial to stop.
// The 500 RPM motors have no encoder: PWM 102 is only a starting estimate for
// 200 RPM. Adjust DRIVE_PWM after measuring the loaded robot's wheel speed.
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

constexpr uint8_t LEFT_IN1 = 25, LEFT_IN2 = 26, LEFT_EN = 33;
constexpr uint8_t RIGHT_IN1 = 27, RIGHT_IN2 = 14, RIGHT_EN = 17;
constexpr uint8_t TRIG = 23, ECHO = 34;
constexpr uint8_t LEFT_IR = 35, RIGHT_IR = 16, START = 32;
constexpr uint8_t SDA_PIN = 21, SCL_PIN = 22;
constexpr int DRIVE_PWM = 102;       // estimated 200 RPM; calibrate on the robot
constexpr int TURN_PWM = 125;
constexpr uint32_t LEFT_90_MS = 580; // calibrate for a loaded 90-degree turn
constexpr uint32_t STEP_TIMEOUT_MS = 25000;
constexpr uint32_t SERVO_WAIT_MS = 1100;
constexpr float WALL_CM = 20.0f;
constexpr int BLACK = LOW;           // change to HIGH if the IR modules are inverted
const int CLOSED_DEG[5] = {15, 15, 15, 35, 35};
const int OPEN_DEG[5] = {100, 100, 100, 110, 110};

Adafruit_PWMServoDriver servos(0x40);
bool finished = false;

void stopMotors() {
  ledcWrite(LEFT_EN, 0);
  ledcWrite(RIGHT_EN, 0);
  digitalWrite(LEFT_IN1, LOW); digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, LOW); digitalWrite(RIGHT_IN2, LOW);
}

bool stopRequested() {
  while (Serial.available()) {
    char command = Serial.read();
    if (command == 'x' || command == 'X') {
      stopMotors();
      Serial.println("Stopped by Serial.");
      return true;
    }
  }
  return false;
}

bool waitMs(uint32_t duration) {
  uint32_t started = millis();
  while (millis() - started < duration) {
    if (stopRequested()) return false;
    delay(5);
  }
  return true;
}

bool drive(int left, int right) {
  // The right motor is mounted in the opposite direction.
  right = -right;
  if (!ledcWrite(LEFT_EN, 0) || !ledcWrite(RIGHT_EN, 0)) return false;
  digitalWrite(LEFT_IN1, LOW); digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, LOW); digitalWrite(RIGHT_IN2, LOW);
  if (left) digitalWrite(left > 0 ? LEFT_IN1 : LEFT_IN2, HIGH);
  if (right) digitalWrite(right > 0 ? RIGHT_IN1 : RIGHT_IN2, HIGH);
  if (!ledcWrite(LEFT_EN, abs(left)) || !ledcWrite(RIGHT_EN, abs(right))) {
    stopMotors();
    return false;
  }
  return true;
}

float distanceCm() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  uint32_t echoUs = pulseIn(ECHO, HIGH, 25000);
  if (echoUs == 0) return -1;
  float cm = echoUs * 0.0343f / 2.0f;
  return cm >= 2 && cm <= 400 ? cm : -1;
}

bool goToWall() {
  float cm = distanceCm();
  if (cm < 0) return false;
  if (cm <= WALL_CM) return true;
  if (!drive(DRIVE_PWM, DRIVE_PWM)) return false;
  uint32_t started = millis();
  while (millis() - started < STEP_TIMEOUT_MS) {
    if (stopRequested()) return false;
    cm = distanceCm();
    if (cm < 0) { stopMotors(); return false; }
    if (cm <= WALL_CM) { stopMotors(); return true; }
    delay(50);
  }
  stopMotors();
  return false;
}

bool turnLeft90() {
  stopMotors();
  if (!drive(-TURN_PWM, TURN_PWM)) return false;
  bool completed = waitMs(LEFT_90_MS);
  stopMotors();
  return completed;
}

bool blackTape() {
  return digitalRead(LEFT_IR) == BLACK && digitalRead(RIGHT_IR) == BLACK;
}

bool goToTape() {
  // See clear floor first, then the next stripe, so a stripe under the IR
  // sensors at the start of this leg cannot count as the destination.
  bool sawClear = !blackTape();
  if (!drive(DRIVE_PWM, DRIVE_PWM)) return false;
  uint32_t started = millis();
  while (millis() - started < STEP_TIMEOUT_MS) {
    if (stopRequested()) return false;
    float cm = distanceCm();
    if (cm < 0 || cm <= WALL_CM) { stopMotors(); return false; }
    if (!blackTape()) sawClear = true;
    else if (sawClear) { stopMotors(); return true; }
    delay(10);
  }
  stopMotors();
  return false;
}

bool setServo(uint8_t channel, int degrees) {
  uint32_t pulseUs = 1000 + 1000UL * degrees / 180;
  uint16_t ticks = pulseUs * 4096UL / 20000UL;
  return servos.setPWM(channel, 0, ticks) == 0;
}

bool drop(uint8_t channel) {
  stopMotors();
  return setServo(channel, OPEN_DEG[channel]) && waitMs(SERVO_WAIT_MS);
}

bool runRoute() {
  // Channels 0, 1, 2: 2-bin, 6-bin, 2-bin.
  // Channels 3, 4: first beam, second beam.
  return goToWall() && turnLeft90() && drop(0) &&
         goToTape() && drop(1) &&
         goToWall() && drop(2) &&
         turnLeft90() && goToWall() &&
         turnLeft90() && drop(3) &&
         turnLeft90() && drop(4);
}

void setup() {
  Serial.begin(115200);
  pinMode(LEFT_IN1, OUTPUT); pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT); pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_EN, OUTPUT); pinMode(RIGHT_EN, OUTPUT);
  pinMode(TRIG, OUTPUT); digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
  pinMode(LEFT_IR, INPUT); pinMode(RIGHT_IR, INPUT);
  pinMode(START, INPUT_PULLUP);
  digitalWrite(LEFT_EN, LOW); digitalWrite(RIGHT_EN, LOW);
  stopMotors();

  if (!ledcAttachChannel(LEFT_EN, 20000, 8, 0) ||
      !ledcAttachChannel(RIGHT_EN, 20000, 8, 1)) {
    Serial.println("Motor PWM setup failed.");
    finished = true;
    return;
  }
  stopMotors();
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.beginTransmission(0x40);
  if (Wire.endTransmission() != 0 || !servos.begin()) {
    Serial.println("PCA9685 not found.");
    finished = true;
    return;
  }
  servos.setPWMFreq(50);
  for (uint8_t channel = 0; channel < 5; ++channel) {
    if (!setServo(channel, CLOSED_DEG[channel])) {
      Serial.println("Servo setup failed.");
      finished = true;
      return;
    }
  }
  Serial.println("Ready. Press START to run; send x to stop.");
}

void loop() {
  if (finished) { stopMotors(); delay(10); return; }
  if (stopRequested()) { finished = true; return; }
  if (digitalRead(START) == LOW) {
    finished = true; // one run per reset
    if (waitMs(200)) { // let the button press settle
      Serial.println(runRoute() ? "Route complete." : "Route stopped or failed.");
    }
    stopMotors();
  }
  delay(10);
}
