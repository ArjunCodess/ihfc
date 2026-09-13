# How the program works

The robot follows a stored sequence. It uses wall distance and a black marker to decide when to stop, and timed wheel movement to turn or correct an offset. It does not use a camera, AI, GPS, wheel encoders or continuous line-following control.

## Route

1. Start at the top right facing left. Drive toward the left wall, stop at the configured clearance, turn left to face down, and open the first two-kit bin.
2. Drive down the left side. After detecting the configured black crossing, apply the outlet correction and open the six-kit bin.
3. Continue to the bottom wall. Stop and open the last two-kit bin.
4. Turn left to face right. Apply the optional approach-lane shift, then drive toward the right wall.
5. Stop at the beam-drop clearance. Send open commands to both grippers, wait for the beams to drop, and remain stopped.

The left-side separator marks entry to the middle area. A configurable offset accounts for the rear sensor's position and where the middle bin should drop its kits.

## Main functions

| Function | What it does |
| --- | --- |
| `setup()` | Configures GPIO and PWM, closes the servos, then waits for the start command. |
| `loop()` | Debounces START, accepts Serial commands, and allows one mission attempt per reset. |
| `runMission()` | Executes the complete route in order. |
| `rangeMm()` | Sends an ultrasonic trigger and converts the echo duration to millimetres. No echo returns an invalid reading. |
| `wallDistance()` | Moves to a wall-distance target. Three stopped readings must be within tolerance before it succeeds. |
| `middleMarker()` | Requires clear floor, then counts debounced black crossings. Stops on a missing marker or unexpected wall. |
| `moveMm()` | Converts a small calibrated forward/reverse offset to running time. This is an estimate without encoders. |
| `turn90()` | Runs the wheels in opposite directions for a calibrated turn duration. |
| `drive()` / `setMotor()` | Set L298N direction inputs and ENA/ENB PWM duty. |
| `servoAngle()` | Converts a requested position to a 50 Hz servo pulse. |
| `releaseLoad()` | Opens one bin or gripper once and leaves it open. The route uses it for the three bins. |
| `releaseBeams()` | Opens both grippers at the same stationary pose. |
| `fail()` / `checkStop()` | Stop the motors on a fault or a connected Serial `x` command. |
| `logLine()` / `logValue()` | Write timestamped INFO, WARN and ERROR messages at 115200 baud. |
| `startupSelfCheck()` | Check configuration, PWM setup, ultrasonic response, IR state and START before arming the mission. |

## What the five servos do

The first three servos each open a complete bin; the program does not count individual kits. Loading the bins with 2, 6 and 2 sets the delivered quantities. The final two servos open the beam grippers. Their commands are issued back-to-back, so exact mechanical simultaneity is not assumed.

The code records which loads have been released and rejects a repeated release. It does not have sensors to confirm that a bin is empty or that a beam landed upright.

## Software fault handling

Missing echoes, target overshoot, travel timeout, invalid settings, invalid servo commands and motor PWM failures stop the mission. The motors then stay stopped until reset. Servos remain at their last commanded positions. Serial stop is checked between operations; one ultrasonic reading can block for up to 25 ms.
