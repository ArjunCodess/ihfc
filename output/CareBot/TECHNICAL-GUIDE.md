# Technical guide

Upload `CareBotESP32/CareBotESP32.ino` to a common 30-pin ESP32-WROOM-32 DevKit. The source repository is [ArjunCodess/ihfc](https://github.com/ArjunCodess/ihfc).

## Automatic startup

GPIO32 is unused. Connecting the battery powers the controller and starts one route after the ESP32 initializes the motor PWM, PCA9685, five servo rest positions, ultrasonic sensor, and two front IR sensors. The robot first moves forward and reads the ultrasonic sensor repeatedly. It stops when the measured wall distance is 20 cm or less. A missing echo stops the motors as a fault.

The program continues through the 2, 6, 2 kit deliveries and beam releases. It stops permanently when the route finishes or a fault occurs. A connected Serial Monitor can send `x` to stop. The main battery switch remains the physical stop.

## Sensor placement

Mount the ultrasonic sensor at the front with a clear view of the wall. Mount both IR sensors side by side at the front and point them down at the arena floor. The black tape checkpoint is accepted only when both inputs report black. GPIO35 has no internal pull-up, so the exact IR module may require an external 3.3 V pull-up or level conversion.

## Servo movement

The PCA9685 uses address `0x40` and 50 Hz output. The commanded positions are:

| Channel | Mechanism | Rest | Release direction and target |
| ---: | --- | ---: | --- |
| 0 | First 2-bin MG995 | 90° | Negative direction to 20° |
| 1 | 6-bin servo | 90° | Positive direction to 160° |
| 2 | Second 2-bin MG995 | 90° | Negative direction to 20° |
| 3 | First beam release | 35° | Positive direction to 110° |
| 4 | Second beam release | 35° | Positive direction to 110° |

These are starting values. Check the horn orientation with empty mechanisms. Change the angles before loading if a linkage binds.

## Runtime logging

Open Serial Monitor at 115200 baud. Each line includes elapsed time and a category:

- `MOTOR` reports left and right PWM commands and stops.
- `ULTRASONIC` reports every measured distance or a missing echo.
- `IR` reports the front-left and front-right state whenever either changes.
- `SERVO` reports the PCA9685 channel and target angle.

## Drive and power

The two left motors share one direction and PWM command. The two right motors share the other. Confirm that the motor driver can carry the combined startup and stall current of two motors per channel. Use a separate regulated 5 to 6 V servo supply on PCA9685 V+, keep PCA9685 VCC at ESP32 logic voltage, and join all grounds. Follow `WIRING.md` for the complete pin map.

The PWM value used for the requested 200 RPM is an estimate based on a nominal 500 RPM motor. Measure the loaded wheel speed. Calibrate the 580 ms left-turn time on the actual four-wheel chassis.