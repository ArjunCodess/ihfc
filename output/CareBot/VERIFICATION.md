# Verification evidence

The sketch keeps the L298N route and open-only deliveries. It uses Adafruit PWM Servo Driver Library for the PCA9685 and has stationary serial tests for individual servo channels. All wall approaches now stop on the first ultrasonic reading at or below 200 mm. Both beam grippers open at one final stop; the robot does not reverse or close the servos after release.

## Actual ESP32 compilation

- Board identifier: `esp32:esp32:esp32`, ESP32 Dev Module for a classic ESP32-WROOM-32 DevKit.
- Core: Espressif Arduino-ESP32 **3.3.11**.
- Warnings: all enabled; no sketch warnings reported.
- Program storage: **314,179 bytes**, 23% of the selected partition.
- Static RAM: **23,824 bytes**, 7%.
- The older output in `evidence/esp32-compile.log` describes the previous DOIT build.

This was a real board-toolchain compilation of the current sketch using the installed Adafruit libraries. The bundle has no uploaded firmware binary because the movement and drop settings still need calibration; build your calibrated source before uploading.

## Portable host regression tests

`tests/test.cpp` includes the packaged sketch using a relative path. `tests/Arduino.h` provides simulated GPIO, time, ultrasonic echoes and Serial input. `tests/Wire.h` and `tests/Adafruit_PWMServoDriver.h` simulate the PCA9685 traffic. Those headers are deliberately outside the sketch folder; they are only for host tests and must not be copied into the Arduino sketch.

With a C++17-capable `g++` on Windows, run `tests/run-tests.ps1`. On a system with a compatible compiler, the equivalent commands from this folder are:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests tests/test.cpp -o carebot-tests
./carebot-tests
```

Coverage includes PCA9685 detection, all five channel writes, I2C failure, both motor directions, motor-control failure, forward-only 20 cm wall stops, missing echoes, timed movement duration, invalid parameters, stop during movement, initially-black and noisy markers, marker timeouts, start debounce, repeated releases, the complete route, and stationary beam release with all delivered servos left open.

The startup tests cover a successful self-check and refusal to arm when the PCA9685 is missing or the ultrasonic sensor returns no echo. Marker tests require both rear IR sensors to report black and reject a reading from only one sensor. All faults use the same ERROR logger and stop both motor-enable outputs.

The older host run in `evidence/host-tests.log` covers the previous revision. The current host test source includes stationary servo tests and a held-start-button check, but this Windows session has no `g++` on PATH, so that host suite has not been rerun. The actual ESP32 compilation above passed.

## What this evidence does not establish

No physical robot tests were performed in this session. The simulator does not model tire slip, inertia, beam weight, grip force, servo travel, wall acoustics or voltage drop under load. It does not establish that the physical route is obstacle-free or that either beam lands upright.

The motor rated voltage and stall current, exact servo models, power system, speed/turn calibration and drop geometry remain to be checked. Use `CALIBRATION.md` to record the physical measurements and run results.

After editing calibration values, recompile and repeat physical tests. The saved logs and checksums describe the supplied version, not later edits.

The showcase page's local links and SVG structure were checked. A visual browser preview was not completed because the browser's policy blocked local-file navigation.
