/*
  CareBot component diagnostic for the 30-pin ESP32 NodeMCU board with USB-C
  and an ESP32-WROOM-32 module. Use GPIO numbers printed on the board.
  Upload this sketch instead of the mission sketch, then open Serial Monitor
  at 115200 baud. Remove kits and beams and raise the drive wheels.

  Commands (send one character; line endings are optional):
    ?           Print help
    b           Board information
    i           Scan the I2C bus
    p           PCA9685 status and planned channel registers
    u           One ultrasonic reading
    r           Front IR sensors
    0..9, A..F   Move one PCA9685 channel (A=10, F=15)
    t           Move channels 0 through 15, one at a time
    !           Arm one short motor or full-actuator test
    L or R      Test left or right motor forward and reverse (requires !)
    T           Rerun the automatic servo-and-motor test
    x           Stop motors, disarm tests, and interrupt an active test

  A PCA9685 register response does not prove a servo is plugged in or moving.
  This wiring has no battery-voltage, servo-voltage, motor-current, wheel-speed,
  or servo-position feedback inputs. Those quantities cannot be measured here.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#if !defined(CONFIG_IDF_TARGET_ESP32)
#error "Select ESP32 Dev Module for the classic ESP32-WROOM-32 NodeMCU board."
#endif

constexpr uint8_t PCA_ADDRESS = 0x40;
constexpr uint8_t SDA_PIN = 21, SCL_PIN = 22;
constexpr uint8_t ULTRA_TRIG_PIN = 23, ULTRA_ECHO_PIN = 34;
constexpr uint8_t LEFT_IR_PIN = 35, RIGHT_IR_PIN = 16;
constexpr uint8_t MOTOR_IN[2][2] = {{25, 26}, {27, 14}};
constexpr uint8_t MOTOR_EN[2] = {33, 17};
constexpr bool MOTOR_INVERT[2] = {false, true};
constexpr uint8_t BLACK_LEVEL = LOW;
constexpr uint8_t MOTOR_TEST_PWM = 145;
constexpr uint32_t MOTOR_TEST_MS = 300;
constexpr uint32_t ULTRA_TIMEOUT_US = 25000;
constexpr uint32_t REPORT_INTERVAL_MS = 2000;
constexpr uint16_t SERVO_LOW_TICKS = 225;
constexpr uint16_t SERVO_CENTER_TICKS = 375;
constexpr uint8_t EXPECTED_50HZ_PRESCALE = 121;

Adafruit_PWMServoDriver pca(PCA_ADDRESS);
bool pcaReady = false;
bool motorPwmReady[2] = {false, false};
bool motorTestArmed = false;
bool testInterrupted = false;
uint32_t lastReportMs = 0;

void prefix(const char *topic) {
  Serial.print('[');
  Serial.print(millis());
  Serial.print(" ms] [");
  Serial.print(topic);
  Serial.print("] ");
}

void motorsOff() {
  for (unsigned side = 0; side < 2; ++side) {
    if (motorPwmReady[side]) {
      if (!ledcWrite(MOTOR_EN[side], 0)) {
        ledcDetach(MOTOR_EN[side]);
        motorPwmReady[side] = false;
        pinMode(MOTOR_EN[side], OUTPUT);
        digitalWrite(MOTOR_EN[side], LOW);
      }
    } else {
      digitalWrite(MOTOR_EN[side], LOW);
    }
    digitalWrite(MOTOR_IN[side][0], LOW);
    digitalWrite(MOTOR_IN[side][1], LOW);
  }
}

bool pcaResponds() {
  Wire.beginTransmission(PCA_ADDRESS);
  return Wire.endTransmission() == 0;
}

void logBoard() {
  prefix("BOARD");
  Serial.print("chip="); Serial.print(ESP.getChipModel());
  Serial.print(" cpu_mhz="); Serial.print(getCpuFrequencyMhz());
  Serial.print(" flash_bytes="); Serial.print(ESP.getFlashChipSize());
  Serial.print(" free_heap_bytes="); Serial.println(ESP.getFreeHeap());
}

void scanI2c() {
  prefix("I2C"); Serial.println("scan_begin sda_gpio=21 scl_gpio=22");
  unsigned found = 0;
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      prefix("I2C");
      Serial.print("address=0x");
      if (address < 16) Serial.print('0');
      Serial.println(address, HEX);
      ++found;
    }
  }
  prefix("I2C"); Serial.print("scan_end responding_addresses="); Serial.println(found);
}

void logUltrasonic() {
  digitalWrite(ULTRA_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG_PIN, LOW);
  uint32_t echoUs = pulseIn(ULTRA_ECHO_PIN, HIGH, ULTRA_TIMEOUT_US);
  prefix("ULTRA");
  Serial.print("trig_gpio=23 echo_gpio=34 echo_us="); Serial.print(echoUs);
  if (echoUs == 0) {
    Serial.println(" distance_mm=NA status=no_echo");
    return;
  }
  float distanceMm = echoUs * 0.343f / 2.0f;
  Serial.print(" distance_mm="); Serial.print(distanceMm, 1);
  Serial.println(distanceMm >= 20 && distanceMm <= 4000
                   ? " status=valid" : " status=out_of_range");
}

void logInputs() {
  int left = digitalRead(LEFT_IR_PIN);
  int right = digitalRead(RIGHT_IR_PIN);
  prefix("INPUT");
  Serial.print("front_left_ir_gpio35_raw="); Serial.print(left);
  Serial.print(" left_ir="); Serial.print(left == BLACK_LEVEL ? "BLACK" : "CLEAR");
  Serial.print(" front_right_ir_gpio16_raw="); Serial.print(right);
  Serial.print(" right_ir="); Serial.println(right == BLACK_LEVEL ? "BLACK" : "CLEAR");
}

void logPca() {
  bool responds = pcaResponds();
  if (!responds) pcaReady = false;
  prefix("PCA");
  Serial.print("address=0x40 ack="); Serial.print(responds ? 1 : 0);
  Serial.print(" initialized="); Serial.print(pcaReady ? 1 : 0);
  if (!responds || !pcaReady) {
    Serial.println(" prescale=NA");
    return;
  }
  uint8_t prescale = pca.readPrescale();
  Serial.print(" prescale="); Serial.print(prescale);
  Serial.println(prescale == EXPECTED_50HZ_PRESCALE
                   ? " target_frequency_hz=50" : " status=frequency_mismatch");
  for (uint8_t channel = 0; channel < 5; ++channel) {
    uint16_t on = pca.getPWM(channel, false);
    uint16_t off = pca.getPWM(channel, true);
    prefix("PCA_CHANNEL");
    Serial.print("channel="); Serial.print(channel);
    Serial.print(" on_register="); Serial.print(on);
    Serial.print(" off_register="); Serial.print(off);
    Serial.print(" output_state=");
    Serial.print((off & 0x1000) ? "FULL_OFF" : "PWM_CONFIGURED");
    Serial.println(" physical_servo_presence=unmeasured");
  }
}

void logMotorCommands() {
  prefix("MOTOR_OUTPUT");
  Serial.print("left_pwm_gpio33=");
  Serial.print(motorPwmReady[0] ? ledcRead(MOTOR_EN[0]) : 0);
  Serial.print(" left_in1_gpio25="); Serial.print(digitalRead(MOTOR_IN[0][0]));
  Serial.print(" left_in2_gpio26="); Serial.print(digitalRead(MOTOR_IN[0][1]));
  Serial.print(" right_pwm_gpio17=");
  Serial.print(motorPwmReady[1] ? ledcRead(MOTOR_EN[1]) : 0);
  Serial.print(" right_in3_gpio27="); Serial.print(digitalRead(MOTOR_IN[1][0]));
  Serial.print(" right_in4_gpio14="); Serial.print(digitalRead(MOTOR_IN[1][1]));
  Serial.println(" wheel_rotation=unmeasured driver_current=unmeasured");
}

void reportEverything() {
  logUltrasonic();
  logInputs();
  logPca();
  logMotorCommands();
}

bool waitWithStop(uint32_t durationMs) {
  uint32_t started = millis();
  while (millis() - started < durationMs) {
    while (Serial.available()) {
      char command = Serial.read();
      if (command == 'x' || command == 'X') {
        testInterrupted = true;
        motorTestArmed = false;
        motorsOff();
        prefix("STOP"); Serial.println("test_interrupted motors_off=1");
        return false;
      }
    }
    delay(10);
  }
  return true;
}

bool setServoAndLog(uint8_t channel, uint16_t ticks) {
  if (!pcaResponds() || pca.setPWM(channel, 0, ticks) != 0) {
    prefix("SERVO"); Serial.println("status=i2c_write_failed");
    pcaReady = false;
    return false;
  }
  uint16_t readback = pca.getPWM(channel, true);
  prefix("SERVO");
  Serial.print("channel="); Serial.print(channel);
  Serial.print(" commanded_off_ticks="); Serial.print(ticks);
  Serial.print(" register_off_ticks="); Serial.print(readback);
  Serial.print(" approximate_pulse_us="); Serial.print(ticks * 20000UL / 4096UL);
  Serial.println(readback == ticks
                   ? " register_match=1 physical_motion=unmeasured"
                   : " register_match=0 physical_motion=unmeasured");
  return readback == ticks;
}

bool testServo(uint8_t channel) {
  motorsOff();
  if (!pcaReady || channel >= 16) {
    prefix("SERVO"); Serial.println("status=driver_not_ready_or_bad_channel");
    return false;
  }
  prefix("SERVO"); Serial.print("test_begin channel="); Serial.println(channel);
  if (!setServoAndLog(channel, SERVO_CENTER_TICKS) || !waitWithStop(600) ||
      !setServoAndLog(channel, SERVO_LOW_TICKS) || !waitWithStop(600) ||
      !setServoAndLog(channel, SERVO_CENTER_TICKS) || !waitWithStop(600)) {
    return false;
  }
  prefix("SERVO");
  Serial.print("test_end channel="); Serial.print(channel);
  Serial.println(" observe_servo_motion_manually=1");
  return true;
}

bool setMotor(uint8_t side, bool forward) {
  if (side >= 2 || !motorPwmReady[side]) return false;
  if (!ledcWrite(MOTOR_EN[side], 0)) return false;
  digitalWrite(MOTOR_IN[side][0], LOW);
  digitalWrite(MOTOR_IN[side][1], LOW);
  bool in1High = forward != MOTOR_INVERT[side];
  digitalWrite(MOTOR_IN[side][in1High ? 0 : 1], HIGH);
  return ledcWrite(MOTOR_EN[side], MOTOR_TEST_PWM);
}

bool runMotorCycle(uint8_t side) {
  if (side >= 2 || !motorPwmReady[side]) {
    motorsOff();
    prefix("MOTOR"); Serial.println("status=pwm_not_ready");
    return false;
  }
  motorsOff();
  for (unsigned direction = 0; direction < 2; ++direction) {
    bool forward = direction == 0;
    if (!setMotor(side, forward)) {
      motorsOff();
      prefix("MOTOR"); Serial.println("status=pwm_or_gpio_command_failed");
      return false;
    }
    prefix("MOTOR");
    Serial.print("side="); Serial.print(side == 0 ? "LEFT" : "RIGHT");
    Serial.print(" direction="); Serial.print(forward ? "FORWARD" : "REVERSE");
    Serial.print(" enable_gpio="); Serial.print(MOTOR_EN[side]);
    Serial.print(" commanded_pwm="); Serial.print(MOTOR_TEST_PWM);
    Serial.print(" duration_ms="); Serial.print(MOTOR_TEST_MS);
    Serial.println(" rotation=unmeasured");
    logMotorCommands();
    if (!waitWithStop(MOTOR_TEST_MS)) return false;
    motorsOff();
    if (!waitWithStop(300)) return false;
  }
  prefix("MOTOR"); Serial.println("test_end motors_off=1 observe_wheel_motion_manually=1");
  return true;
}

void testMotor(uint8_t side) {
  if (!motorTestArmed) {
    prefix("MOTOR"); Serial.println("test_refused arm_with_exclamation_mark_first=1");
    return;
  }
  motorTestArmed = false;
  runMotorCycle(side);
}

void testAllActuators() {
  motorTestArmed = false;
  testInterrupted = false;
  motorsOff();
  prefix("ALL_TEST"); Serial.println("test_begin servos_0_to_15_then_both_motors=1");
  bool servoCommandsOk = true, motorCommandsOk = true;
  for (uint8_t channel = 0; channel < 16; ++channel) {
    if (!testServo(channel)) {
      motorsOff();
      servoCommandsOk = false;
      if (testInterrupted) {
        prefix("ALL_TEST"); Serial.println("test_interrupted_during_servo_sweep=1 motors_off=1");
        return;
      }
      prefix("ALL_TEST"); Serial.println("servo_sweep_failed_continuing_to_motor_tests=1");
      break;
    }
  }
  for (uint8_t side = 0; side < 2; ++side) {
    if (!runMotorCycle(side)) {
      motorsOff();
      motorCommandsOk = false;
      if (testInterrupted) {
        prefix("ALL_TEST"); Serial.println("test_interrupted_during_motor_test=1 motors_off=1");
        return;
      }
      prefix("ALL_TEST"); Serial.println("motor_test_failed_continuing_to_other_side=1");
    }
  }
  motorsOff();
  prefix("ALL_TEST");
  Serial.print("test_end servo_command_status=");
  Serial.print(servoCommandsOk ? "PASS" : "ERROR");
  Serial.print(" motor_command_status=");
  Serial.print(motorCommandsOk ? "PASS" : "ERROR");
  Serial.println(" motors_off=1 physical_motion=unmeasured");
}

int channelFromCommand(char command) {
  if (command >= '0' && command <= '9') return command - '0';
  if (command >= 'A' && command <= 'F') return command - 'A' + 10;
  return -1;
}

void printHelp() {
  prefix("HELP"); Serial.println("? help | b board | i I2C | p PCA | u ultrasonic | r front IR sensors");
  prefix("HELP"); Serial.println("All servos and both motors test automatically after boot; T reruns the full test.");
  prefix("HELP"); Serial.println("0..9,A..F one servo | t all 16 servos");
  prefix("HELP"); Serial.println("! then L/R one motor | x stop. Raise wheels before motor tests.");
  prefix("HELP"); Serial.println("Inputs and PCA registers report every 2 seconds. Motors are off by default.");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  for (unsigned side = 0; side < 2; ++side) {
    pinMode(MOTOR_IN[side][0], OUTPUT);
    pinMode(MOTOR_IN[side][1], OUTPUT);
    pinMode(MOTOR_EN[side], OUTPUT);
  }
  motorsOff();
  pinMode(ULTRA_TRIG_PIN, OUTPUT);
  digitalWrite(ULTRA_TRIG_PIN, LOW);
  pinMode(ULTRA_ECHO_PIN, INPUT);
  pinMode(LEFT_IR_PIN, INPUT);
  pinMode(RIGHT_IR_PIN, INPUT);
  for (unsigned side = 0; side < 2; ++side) {
    motorPwmReady[side] = ledcAttachChannel(MOTOR_EN[side], 20000, 8, side);
    if (motorPwmReady[side] && !ledcWrite(MOTOR_EN[side], 0)) {
      ledcDetach(MOTOR_EN[side]);
      motorPwmReady[side] = false;
      pinMode(MOTOR_EN[side], OUTPUT);
      digitalWrite(MOTOR_EN[side], LOW);
    }
    prefix("MOTOR_SETUP");
    Serial.print("side="); Serial.print(side == 0 ? "LEFT" : "RIGHT");
    Serial.print(" pwm_ready="); Serial.println(motorPwmReady[side] ? 1 : 0);
  }
  motorsOff();
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  logBoard();
  scanI2c();
  if (pcaResponds() && pca.begin()) {
    pca.setPWMFreq(50);
    pcaReady = pcaResponds() && pca.readPrescale() == EXPECTED_50HZ_PRESCALE;
  }
  prefix("PCA_SETUP");
  Serial.print("ready="); Serial.println(pcaReady ? 1 : 0);
  prefix("LIMIT");
  Serial.println("servo_vplus=battery_voltage=motor_current=wheel_speed=servo_position=oe_level=unmeasured");
  printHelp();
  reportEverything();
  lastReportMs = millis();
  testAllActuators();
  reportEverything();
  lastReportMs = millis();
}

void loop() {
  if (millis() - lastReportMs >= REPORT_INTERVAL_MS) {
    reportEverything();
    lastReportMs = millis();
  }
  while (Serial.available()) {
    char command = Serial.read();
    if (command == '\n' || command == '\r' || command == ' ') continue;
    int channel = channelFromCommand(command);
    if (channel >= 0) {
      testServo(uint8_t(channel));
    } else if (command == 't') {
      for (uint8_t ch = 0; ch < 16; ++ch) if (!testServo(ch)) break;
    } else if (command == 'T') {
      testAllActuators();
    } else if (command == '!') {
      motorTestArmed = true;
      prefix("MOTOR"); Serial.println("armed=1 send_L_R_or_T_with_wheels_raised=1");
    } else if (command == 'L' || command == 'R') {
      testMotor(command == 'L' ? 0 : 1);
    } else if (command == 'x' || command == 'X') {
      testInterrupted = true;
      motorTestArmed = false;
      motorsOff();
      prefix("STOP"); Serial.println("motors_off=1 tests_disarmed=1");
    } else if (command == 'b') {
      logBoard();
    } else if (command == 'i') {
      scanI2c();
    } else if (command == 'p') {
      logPca();
    } else if (command == 'u') {
      logUltrasonic();
    } else if (command == 'r') {
      logInputs();
    } else if (command == '?') {
      printHelp();
    } else {
      prefix("HELP"); Serial.println("unknown_command send_question_mark_for_help=1");
    }
  }
  delay(10);
}
