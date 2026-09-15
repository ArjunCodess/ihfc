# How the program works

The robot follows a stored sequence. It uses wall distance and a black marker to decide when to stop, and timed wheel movement to turn or correct an offset. It does not use a camera, AI, GPS, wheel encoders or continuous line-following control.

## Route

1. Start at the top right facing left. Drive toward the left wall until the ultrasonic reads 20 cm or less, turn left to face down, and open the first two-kit bin.
2. Drive down the left side. After detecting the configured black crossing, apply the outlet correction and open the six-kit bin.
3. Continue to the bottom wall. Stop and open the last two-kit bin.
4. Turn left to face right. Apply the optional approach-lane shift, then drive toward the right wall.
5. Stop at the same 20 cm threshold. Send open commands to both grippers, wait for the beams to drop, and remain stopped.

The left-side separator marks entry to the middle area. A configurable offset accounts for the rear sensor's position and where the middle bin should drop its kits.

## Main functions

| Function | What it does |
| --- | --- |
| `setup()` | Configures GPIO and motor PWM, initializes the PCA9685, closes the servos, then waits for the start command. |
| `loop()` | Debounces START, accepts Serial commands, and allows one mission attempt per reset. |
| `runMission()` | Executes the complete route in order. |
| `rangeMm()` | Sends an ultrasonic trigger and converts the echo duration to millimetres. No echo returns an invalid reading. |
| `approachWall()` | Drives forward while valid ultrasonic readings are above 200 mm. Stops on the first reading at or below 200 mm. |
| `middleMarker()` | Requires clear floor, then counts debounced black crossings. Stops on a missing marker or unexpected wall. |
| `moveMm()` | Converts a small calibrated forward/reverse offset to running time. This is an estimate without encoders. |
| `turn90()` | Runs the wheels in opposite directions for a calibrated turn duration. |
| `drive()` / `setMotor()` | Set L298N direction inputs and ENA/ENB PWM duty. |
| `initializeServoDriver()` | Detects the PCA9685 at `0x40` and configures its shared output frequency to 50 Hz. |
| `servoAngle()` | Converts a requested angle to a pulse count and writes the assigned PCA9685 channel. |
| `testServoChannel()` | Tests one PCA9685 socket from Serial with both motor enables off. |
| `releaseLoad()` | Opens one bin or gripper once and leaves it open. The route uses it for the three bins. |
| `releaseBeams()` | Opens both grippers at the same stationary pose. |
| `fail()` / `checkStop()` | Stop the motors on a fault or a connected Serial `x` command. |
| `logLine()` / `logValue()` | Write timestamped INFO, WARN and ERROR messages at 115200 baud. |
| `startupSelfCheck()` | Check configuration, motor PWM, PCA9685 response, ultrasonic response, IR state and START before arming the mission. |

## What the five servos do

All servo signals come from one PCA9685 board over ESP32 GPIO21/SDA and GPIO22/SCL. Channels 0, 1 and 2 open the three complete bins; the program does not count individual kits. Loading those bins with 2, 6 and 2 sets the delivered quantities. Channels 3 and 4 open the right and front beam grippers. Their commands are issued back-to-back, so exact mechanical simultaneity is not assumed.

The mission maps five planned servo functions, but only two servos are connected at present. Send a channel character `0` through `9` or `A` through `F` to test a socket with the motors off. This command moves the selected channel between 225 and 375 PCA9685 ticks and restores its mission closed position if it is one of channels 0 through 4. Use empty mechanisms for this test.

The code records which loads have been released and rejects a repeated release. It does not have sensors to confirm that a bin is empty or that a beam landed upright.

## Software fault handling

Missing echoes, a missing PCA9685, failed I2C writes, travel timeout, invalid settings, invalid servo commands and motor PWM failures stop the mission. The motors then stay stopped until reset. Servos remain at their last commanded positions. Serial stop is checked between operations; one ultrasonic reading can block for up to 25 ms.
