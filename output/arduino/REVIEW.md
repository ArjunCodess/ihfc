Latest behavior: each delivery servo opens and stays open. Both beam grippers open at one final stop, and the robot stays there without reversing.

# Code review and verification

Reviewed the complete CareBot sketch for the DOIT ESP32 DEVKIT V1, L298N, two 12 V, 500 RPM geared motors, three bin servos, two gripper servos, front ultrasonic sensor, rear IR sensor and one start button.

## Errors fixed

- Timed forward moves previously started their timer before waiting for the first ultrasonic measurement. Small offsets could lose most or all of their commanded movement. The initial reading now happens while stopped, and the movement timer starts when the motors run. Subsequent readings only start when enough time remains before the stop deadline.
- Wall positioning previously accepted any reading beyond the target in the travel direction. A substantial overshoot could authorize a drop at the wrong location. A successful stop now requires three stationary readings within tolerance; an overshoot outside tolerance stops the run.
- Marker seeking could begin moving before obtaining a fresh wall reading. It now checks clearance while stopped before driving toward the marker.
- The start button previously waited 40 ms without checking that it remained pressed. It now requires an uninterrupted press and must first be released after power-up.
- Motor PWM writes were unchecked, and a left-motor control failure could allow the right motor to continue. PWM failures now abort both motors. Stop handling tracks which pins were attached, and detaches PWM before forcing a pin low if its stop write fails.
- Serial output now uses timestamped INFO, WARN and ERROR messages. Startup blocks the mission on duplicate pins, PWM setup failures, servo-command failures, or a missing ultrasonic echo.
- Timed distances and servo array indices lacked complete validation. Invalid numeric distances, impossible targets, invalid servo indices/angles and invalid beam drop settings now stop execution. Compile-time checks catch invalid speed, pulse, marker-count and turn-time settings.
- Zero-speed straight movement could apply a configured motor trim. Zero now explicitly stops both motors.

## Verification

The actual Arduino build passed with Arduino-ESP32 3.3.5, board `esp32:esp32:esp32doit-devkit-v1`, and all warnings enabled. No sketch warnings were reported.

The current source passes a warning-clean C++17 compile against the regression harness. Tests cover startup self-check success and ultrasonic failure, motor direction and failures, timed motion duration, missing echoes, wall clearance and overshoot, invalid parameters, Serial stop during motion, start-button debounce, noisy and initially-black markers, marker and wall timeouts, one-time releases, and refusal to restart a completed or aborted run.

Test sources are in `tmp/carebot-code-test` under the workspace. `configured.ino` there is an obsolete test copy from the previous two-position beam sequence; do not upload it. Arduino build output and the final compile log are in `tmp/carebot-toolchain`.

## Physical limits still unresolved

This verifies compilation and software behavior, not navigation accuracy on a real field. No firmware was uploaded to a robot.

- The L298N module's current rating must exceed the measured motor demand, including startup and stall. Its bridge voltage drop also reduces the voltage reaching the motors.
- Speed, turn duration, servo angles, outlet corrections, stopping clearance and beam drop clearance still require measurements. The source retains example motion values and an example final beam wall clearance.
- One binary rear IR sensor detects crossing markers but cannot continuously correct heading. The first transverse separator is the middle area's entrance, not its centre. Sensor polling can miss a narrow marker at excessive speed, especially during a blocking ultrasonic reading.
- The ultrasonic sensor must see the intended wall, not a carried beam, released load or another obstacle. Its reading can block for up to 25 ms; Serial stop is serviced between such readings.
- Reverse motion has no rear obstacle measurement. Turning has no side-clearance measurement. Both require the loaded robot's path to be physically clear.
- Each flap release assumes its preloaded 2/6/2 group falls out. There is no sensor to verify an empty bin or confirm that a released beam stands upright.

Use the production sketch at `CareBotESP32/CareBotESP32.ino` with the wiring and calibration procedure in `README.md`.

