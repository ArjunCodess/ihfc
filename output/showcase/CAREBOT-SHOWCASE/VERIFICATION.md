# Verification evidence

This pack contains the current L298N and PCA9685 sketch with open-only deliveries and timestamped Serial logging. Both beam grippers open at one final stop; the robot does not reverse or close the servos after release.

## Actual ESP32 compilation

- Board identifier: `esp32:esp32:esp32doit-devkit-v1`, DOIT ESP32 DEVKIT V1.
- Core: Espressif Arduino-ESP32 **3.3.5**.
- Warnings: all enabled; no sketch warnings reported.
- Program storage: **310,587 bytes**, 23% of the selected partition.
- Static RAM: **23,712 bytes**, 7%.
- Saved output: `evidence/esp32-compile.log`.

This was a real board-toolchain compilation of the current L298N and PCA9685 revision. The same source also passes a warning-clean C++17 compile against the host harness. The bundle has no compiled firmware binary because the movement and drop settings still need calibration; build your calibrated source before uploading.

## Portable host regression tests

`tests/test.cpp` includes the actual packaged sketch using a relative path. `tests/Arduino.h` provides simulated GPIO, time, ultrasonic echoes and Serial input, while `tests/Wire.h` simulates the PCA9685 I2C traffic. Those headers are deliberately outside the sketch folder; they are only for host tests and must not be copied into the Arduino sketch.

With a C++17-capable `g++` on Windows, run `tests/run-tests.ps1`. On a system with a compatible compiler, the equivalent commands from this folder are:

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests tests/test.cpp -o carebot-tests
./carebot-tests
```

Coverage includes PCA9685 detection, all five channel writes, I2C failure, both motor directions, motor-control failure, forward and reverse wall targets, missing echoes, wall overshoot, timed movement duration, invalid parameters, stop during movement, initially-black and noisy markers, marker timeouts, start debounce, repeated releases, the complete route, and stationary beam release with all delivered servos left open.

The startup tests cover a successful self-check and refusal to arm when the PCA9685 is missing or the ultrasonic sensor returns no echo. Marker tests require both rear IR sensors to report black and reject a reading from only one sensor. All faults use the same ERROR logger and stop both motor-enable outputs.

The packaged tests were re-run after making the paths portable. Their output is saved in `evidence/host-tests.log`.

## What this evidence does not establish

No physical robot tests were performed in this session. The simulator does not model tire slip, inertia, beam weight, grip force, servo travel, wall acoustics or voltage drop under load. It does not establish that the physical route is obstacle-free or that either beam lands upright.

The motor rated voltage and stall current, exact servo models, power system, speed/turn calibration and drop geometry remain to be checked. Use `CALIBRATION.md` to record the physical measurements and run results.

After editing calibration values, recompile and repeat physical tests. The saved logs and checksums describe the supplied version, not later edits.

The showcase page's local links and SVG structure were checked. A visual browser preview was not completed because the browser's policy blocked local-file navigation.
