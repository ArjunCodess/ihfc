# PCA9685 servo driver implementation plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Move all five CareBot servo signals from ESP32 LEDC pins to PCA9685 channels 0 through 4.

**Architecture:** The sketch will control the PCA9685 directly over I²C at address `0x40`, using GPIO21 for SDA and GPIO22 for SCL. A small driver layer will initialize 50 Hz output, probe the board during startup, and translate the existing servo angles into 12-bit PCA9685 pulse counts.

**Tech Stack:** Arduino-ESP32 3.x, `Wire`, PCA9685, C++17 host regression harness

---

### Task 1: Add PCA9685 regression support

**Files:**
- Create: `output/showcase/CAREBOT-SHOWCASE/tests/Wire.h`
- Modify: `output/showcase/CAREBOT-SHOWCASE/tests/test.cpp`

- [x] **Step 1: Add a Wire mock that records PCA9685 channel pulses**

Create a mock with `begin`, `beginTransmission`, `write`, and `endTransmission`. Record five-byte channel writes beginning at register `0x06`, and expose a failure flag for I²C error tests.

- [x] **Step 2: Replace direct-pin servo assertions**

Assert that opening servo indexes 0 through 4 writes their expected 12-bit pulse counts to PCA9685 channels 0 through 4.

- [x] **Step 3: Add startup and runtime failure cases**

Verify that setup refuses to arm when the PCA9685 probe fails and that a failed servo-channel write calls `fail()` and stops both motor enables.

### Task 2: Implement direct PCA9685 control

**Files:**
- Modify: `output/arduino/CareBotESP32/CareBotESP32.ino`
- Modify: `output/showcase/CAREBOT-SHOWCASE/CareBotESP32/CareBotESP32.ino`

- [x] **Step 1: Replace servo GPIO constants**

Use `SERVO_CHANNELS = {0, 1, 2, 3, 4}`, `PCA9685_ADDRESS = 0x40`, `SDA_PIN = 21`, and `SCL_PIN = 22`.

- [x] **Step 2: Add I²C register writes**

Use `Wire` to probe address `0x40`, put the PCA9685 to sleep, set prescale `121` for approximately 50 Hz with the default 25 MHz oscillator, wake it, and enable restart plus auto-increment.

- [x] **Step 3: Route every servo command through the driver**

Convert the existing 1000 to 2000 microsecond pulse range into PCA9685 counts with `pulseUs * 4096 / 20000`, then write the selected channel's four LED registers.

- [x] **Step 4: Extend startup diagnostics**

Fail startup if the PCA9685 does not acknowledge. Log successful detection and state that servo positions still cannot be verified without feedback.

### Task 3: Update hardware documentation

**Files:**
- Modify: `output/arduino/README.md`
- Modify: `output/showcase/CAREBOT-SHOWCASE/WIRING.md`
- Modify: `output/showcase/CAREBOT-SHOWCASE/TECHNICAL-GUIDE.md`
- Modify: `output/showcase/CAREBOT-SHOWCASE/COMPONENTS.md`
- Modify: `output/showcase/CAREBOT-SHOWCASE/CODE-EXPLAINED.md`
- Modify: `docs/CAREBOT-FINAL-ESP32.tex`

- [x] **Step 1: Document I²C and channel wiring**

Specify GPIO21 to SDA, GPIO22 to SCL, PCA9685 channels 0 through 4 to the three bin servos and two beam servos, VCC to ESP32 3.3 V, and OE to ground.

- [x] **Step 2: Document servo power**

Connect the regulated servo supply to PCA9685 V+, connect every servo power lead to the PCA9685 power rail, and join servo-supply, PCA9685, ESP32, sensor, and L298N grounds. Do not power the servos from ESP32 3.3 V or the PCA9685 VCC logic pin.

### Task 4: Verify and publish

**Files:**
- Test: `output/showcase/CAREBOT-SHOWCASE/tests/test.cpp`

- [x] **Step 1: Compile the regression harness**

Run the existing ESP32 cross-compiler with `-std=c++17 -Wall -Wextra -Werror`. Expect no diagnostics.

- [x] **Step 2: Check generated changes**

Run `git diff --check`, confirm no stale direct-servo GPIO references remain, and confirm both sketch copies have matching normalized content.

- [x] **Step 3: Commit and push**

Commit with `Move servos to PCA9685 driver` and push `main` to `origin`.
