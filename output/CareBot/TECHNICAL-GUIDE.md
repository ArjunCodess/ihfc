# CareBot ESP32 code

Open `CareBotESP32/CareBotESP32.ino` in Arduino IDE. This implements the new route in your message, including the photographed chassis with a transverse front beam and a lengthwise right beam. The old document's Uno, camera and 45-degree branch are not used.

The sketch uses a DOIT ESP32 DEVKIT V1, an L298N module, a PCA9685 servo driver at I2C address `0x40`, and two dual-shaft 12 V, 500 RPM DC geared motors. Confirm that the L298N can handle each motor's measured stall current. There is one start button and no physical stop button; Serial x remains available while connected. Select **DOIT ESP32 DEVKIT V1**, install **esp32 by Espressif Systems 3.x**, and use Serial Monitor at **115200 baud**. The firmware talks to the PCA9685 through the built-in `Wire` library, so no extra servo library is needed.

## What it does

1. Start at the top right facing left. Drive to the left wall, stop with clearance at the top left, turn left to face down, and open bin A containing two kits.
2. Drive down the left side until both rear IR sensors detect the configured transverse black marker. Correct the outlet position into the left-middle delivery area, then open the middle bin containing six kits.
3. Continue down to the bottom wall and open bin B containing two kits at the bottom left.
4. Turn left to face right. If necessary, shift up to align the right beam with the top boundary of quarantine, then continue right toward the right wall beside the bottom-right beam area.
5. Stop at `BEAM_DROP_WALL_MM` from the right wall and command both beam grippers open, letting both beams drop from the same robot position.
6. Stay stopped with all five servos open. Reset closes the servos before reloading for another run.

The main route has two 90-degree left turns, at the top left and bottom left. Optional lane correction adds a turn out and a turn back. Each kit servo releases a whole preloaded group once; there is no kit counter or individual dispensing mechanism. The sketch uses a gravity flap. A throwing linkage would need different servo positions and motion timing.

On this image, the first horizontal black separator on the left is the entrance to the middle area, not a marker at its centre. The default first-marker setting uses that entrance. Set `MIDDLE_OUTLET_CORRECTION_MM` so the outlet lands inside the middle area; a positive correction moves farther down, and a negative correction reverses up. The continuous vertical black line alone cannot identify the middle stopping point.

The complete kit-and-beam route is enabled. `BEAM_DROP_WALL_MM = 220` is an example wall clearance, just like the kit wall stops; calibrate it on the actual robot. Each servo only opens when delivering. There is no automatic re-closing, beam repositioning, or withdrawal after dropping the beams.

## Wiring

Use [WIRING.md](WIRING.md) for the complete DOIT ESP32 DEVKIT V1 label-to-GPIO map, PCA9685 channel assignments, power connections, and the ultrasonic ECHO divider. The sketch and that wiring sheet are the two files to check before connecting hardware.

Connect OUT1/OUT2 to the left motor and OUT3/OUT4 to the right motor. Remove the ENA and ENB jumpers, then connect ENA to GPIO33 and ENB to GPIO17 for PWM speed control. Connect the 12 V motor supply to the L298N motor-supply terminal and join the supply, L298N and ESP32 grounds. Never connect 12 V to the module's 5 V terminal or directly to the ESP32. Follow the exact module's instructions for its 5 V regulator jumper.

Check that the L298N module's continuous-current and startup/stall-current capability matches the motors. The motors are rated 12 V and 500 RPM, but their stall current still needs measurement or a trustworthy datasheet. The L298N also drops some voltage, so the motors will receive less than the battery voltage while running.

`WIRING.md` is the pin and power reference. In particular, VCC is PCA9685 logic power, while V+ takes the separate regulated servo supply; they are not interchangeable. The wiring sheet also shows the 5 V ultrasonic ECHO divider and the 3.3 V limit for both IR inputs. Keep both L298N enable jumpers removed so the ESP32 controls when the motors run.

## Serial logger and startup check

Open Serial Monitor at **115200 baud** before resetting the ESP32. Messages contain a timestamp and severity, for example `[912 ms] [INFO] Startup self-check passed.` Every reported `ERROR` stops both L298N enable outputs and prevents the mission from continuing.

At startup, the sketch checks for duplicate GPIO assignments, initializes both motor PWM channels, detects and configures the PCA9685 for 50 Hz, commands all five driver channels closed, requires one valid ultrasonic reading from 20 to 4000 mm, rechecks the PCA9685 response, reports both IR input states, and reports whether START is held. A missing driver or failed I2C write is logged as an `ERROR`, both motor enables are disabled, and the mission cannot start. Place a solid target within the ultrasonic sensor's range before resetting the board.

The L298N has no feedback connection, so the ESP32 cannot confirm motor rotation, motor current, or whether driver power is present. Standard positional servos also provide no position feedback. The logger reports these limits as `WARN` messages. Test motor direction with the chassis raised and confirm servo movement visually.

The front ultrasonic must see past or below the loaded front beam. If it sees the beam itself, the wall route cannot work. Mount both rear IR sensors side by side and point them down at the floor. The marker counts only when both sensors report black for `LINE_STABLE_MS`. The pair confirms crossings but does not continuously correct steering or recover position after a bad turn.

## Beam placement and the photograph

Use the orientation of your latest field image: start at the top right, kits down the left side, and quarantine at the bottom right. After traveling down the kit side and turning left, the robot faces right/east; its right side faces down/south. The right beam runs horizontally along quarantine's top boundary, and the front beam runs vertically along its left boundary. The two outer walls complete the corner. The image labels the field 1143 mm wide and 1181 mm high, but those dimensions do not establish gripper offsets or exact sensor stop distances.

At the final stop, both grippers receive their open commands back-to-back. The robot waits for the beams to drop and remains stopped. There is no separate stop position for each beam and no reverse movement after release.

`BEAM_LANE_SHIFT_MM` can align the approach lane above the bottom-right area before the final drive; its default is zero. `BEAM_DROP_WALL_MM` determines the one final stop measured from the front ultrasonic to the right wall. Both beam mounts must therefore put their loads over the intended landing positions at that same stop. Opening the jaws must leave both beams clear of the robot without requiring it to withdraw. A single opening command cannot correct a bad mounting position or guarantee an upright landing.

## Calibration

Start with the wheels raised and empty mechanisms. On boot, all five servos move to their closed positions; keep fingers clear and load afterward. Release START after power-up, then hold it for at least 40 ms, or send `s`, to begin. A button held during power-up or a brief contact bounce does not start the robot. Send `x` over the connected Serial Monitor to stop; restart requires a reset. An accessible main power switch can disconnect the battery when the robot is running without a computer. The Serial stop command halts the motors and leaves the grippers holding their current positions. It is a software stop, not a power disconnect.

1. Check motor direction using `INVERT_LEFT` and `INVERT_RIGHT`. Adjust `LEFT_TRIM` and `RIGHT_TRIM` for straight travel, then measure forward and reverse millimetres per second at `DRIVE_PWM`.
2. Measure `TURN_LEFT_MS` and `TURN_RIGHT_MS` for actual 90-degree turns with the full load. The example 580 ms is not a measured turn for your robot.
3. Adjust `CLOSED_DEG`, `OPEN_DEG`, pulse limits and `RELEASE_MS` with empty flaps first, then the actual 2/6/2 kit loads. The flap stays open after release; reset it only before reloading.
4. Set wall clearances from the ultrasonic face, accounting for the front beam's overhang and stopping distance. Tune first and final kit outlet corrections to land inside their areas.
5. Check `BLACK_LEVEL` and `MIDDLE_MARKER_NUMBER`. Both sensors must see clear floor before the marker and then see black together; a continuous longitudinal line under only one sensor does not count. The sensors sample about every 2 ms except during an ultrasonic reading, which can block for 25 ms. Use a crossing speed and marker width that leave time for the 20 ms debounce.
6. Set `MIDDLE_OUTLET_CORRECTION_MM`. If the outlet is 80 mm ahead of the rear sensor and should drop on the line, begin with approximately -80 mm and adjust for coasting. Zero means the outlet's ahead-of-line position is already the desired drop point. Reverse corrections need a clear path because the IR is not an obstacle detector.
7. Measure the beam lane and the single final wall clearance. Enter `BEAM_LANE_SHIFT_MM` and `BEAM_DROP_WALL_MM`, and confirm both beams fall clear when the robot stays still.

Every travel leg has a timeout. Missing ultrasonic echoes stop travel instead of being treated as open space. Three stationary readings within the configured distance tolerance confirm a wall stop. Overshooting outside that tolerance stops the run instead of authorizing a delivery, but ultrasonic cannot distinguish a wall from another object, so this assumes a clear route with visible walls. Turns and lateral moves remain sensitive to wheel slip and battery level without encoders or a gyro.

## API references

Validation: host C++ regression tests passed with warnings treated as errors, including the complete kit-and-beam route, flaps staying open, and stationary beam release. The actual ESP32 build is recorded in `REVIEW.md`. No physical robot testing has been performed.

Motor PWM setup follows [Espressif's Arduino-ESP32 LEDC API](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html). Motor control follows the [ST L298 datasheet](https://www.st.com/resource/en/datasheet/l298.pdf). The two motor-enable outputs use ESP32 PWM; the five servos use PCA9685 channels 0 through 4 over I2C. This pin map is not for an ESP32-CAM, C3 or S3.


## Gripper options and current quantities

Use two gripper mechanisms, one at the front and one at the right, each with one positional servo. These are the same two gripper servos already counted in the total of five; the other three operate the bins. Buy or print the mechanisms, not both sets.

The Amazon listing and Thingiverse model could not be read well enough to verify jaw opening, servo inclusion, mounting dimensions or load capacity. Do not assume the Amazon package includes an MG90S. For a printed mechanism, check the exact servo body and horn fit before printing both copies. Neither option is approved for the beam load from a product name alone.

For the documented 60 × 20 mm beam cross-section, jaws gripping across the narrow faces need to open beyond 20 mm with clearance. Grip near the beam's lengthwise centre and use broad padded contacts to resist rotation. A retaining lip or shaped jaw can carry weight without relying only on friction, but it must withdraw completely when opened so the beam can fall free. Test one mechanism with the actual beam, through turns and release, before duplicating it.

Current electronics: one DOIT ESP32 DEVKIT V1, one L298N module, one PCA9685 16-channel servo driver, two dual-shaft 12 V, 500 RPM DC geared motors, five servos, one front ultrasonic sensor, two rear downward IR sensors, and one start button. Add battery and suitable regulated supplies, wiring and a main power switch. No separate stop pushbutton is used.

## Remaining build items

Keep five positional servos total, one PCA9685 servo driver, three flap mechanisms, two beam gripper mechanisms, one DOIT ESP32 DEVKIT V1, one front ultrasonic sensor, two rear IR sensors and one start button. Add two wheels matching the motor shafts, one caster, two motor mounts and the chassis. The second shaft on each motor does not require a second motor driver or an additional drive wheel.

The remaining power parts are a motor-compatible battery pack and matching charger, regulated power for the ESP32 and five servos, a main power switch, and suitable connectors and wiring. Final regulator current depends on the servo models and loads. Include the ultrasonic echo divider and common ground described above. Do not power the servos through the ESP32 board.

Measure wheel diameter and loaded speed before setting timed movements. The 500 RPM rating is not a command to drive at full speed: use PWM for slower approach and marker crossings, and check that the loaded robot still starts and turns reliably. All existing speed and turn-time constants are uncalibrated examples.


