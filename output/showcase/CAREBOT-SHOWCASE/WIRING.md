# Wiring sheet

This pin map is for a DOIT ESP32 DEVKIT V1, L298N and PCA9685 servo driver. Use the GPIO numbers printed on the board, not the physical pin positions. GPIO34 and GPIO35 are input-only pins, which is correct here because they receive the ultrasonic ECHO and left IR signals.

| ESP32 GPIO | Connect to |
| ---: | --- |
| 25 | L298N IN1 |
| 26 | L298N IN2 |
| 27 | L298N IN3 |
| 14 | L298N IN4 |
| 33 | L298N ENA, remove its jumper |
| 17 | L298N ENB, remove its jumper |
| 23 | Ultrasonic TRIG |
| 34 | Ultrasonic ECHO through the divider below |
| 35 | Left rear IR sensor digital output, at a compatible logic voltage |
| 16 | Right rear IR sensor digital output, at a compatible logic voltage |
| 21 | PCA9685 SDA |
| 22 | PCA9685 SCL |
| 32 | Start button; other terminal to GND |

## Motor driver and power

- OUT1/OUT2 connect to the left motor. OUT3/OUT4 connect to the right motor.
- Connect battery positive to the L298N 12 V motor-supply terminal. Connect battery negative to L298N GND.
- Remove the ENA and ENB jumpers. GPIO33 and GPIO17 supply PWM to those pins.
- Never connect 12 V to the module's 5 V terminal or directly to the ESP32. Follow the exact module's instructions for its 5 V regulator jumper.
- Confirm that the L298N module can handle each motor's startup and stall current.
- Connect a separate regulated servo rail to PCA9685 V+. The regulator must suit the servos' specified voltage and combined current; its rating cannot be finalized until the servo models and loads are known.
- Power the ESP32 DevKit through its supported USB or regulated board-power input. Follow that board's requirements; do not connect a raw motor battery to its 3.3 V pin.
- Join PCA9685 GND, the servo-supply negative, ESP32 GND, sensor grounds and L298N GND.

## Ultrasonic ECHO divider

For a sensor whose ECHO output is 5 V:

```text
Sensor ECHO ---- 10 kΩ ----+---- ESP32 GPIO34
                         |
                        15 kΩ
                         |
                        GND
```

This gives approximately 3 V at GPIO34 for a 5 V ECHO output. Power an HC-SR04 according to its specification. Mount it so it can see the wall past or below the front beam.

## IR sensors

GPIO35 has no internal pull-up. An open-collector IR output may need an external pull-up to 3.3 V; a 5 V push-pull output needs level conversion. Mount both sensors side by side at the rear and keep them close enough to the floor to distinguish tape from the surrounding surface. The code confirms a transverse marker only while both sensors report black.

## PCA9685 and servos

| Connection | Destination |
| --- | --- |
| ESP32 GPIO21 | PCA9685 SDA |
| ESP32 GPIO22 | PCA9685 SCL |
| ESP32 3.3 V | PCA9685 VCC logic power |
| Common GND | PCA9685 GND and OE |
| Regulated servo supply positive | PCA9685 V+ |
| PCA9685 channel 0 | Bin A servo, 2 kits |
| PCA9685 channel 1 | Middle-bin servo, 6 kits |
| PCA9685 channel 2 | Bin B servo, 2 kits |
| PCA9685 channel 3 | Right beam gripper servo |
| PCA9685 channel 4 | Front beam gripper servo |

Leave the PCA9685 address jumpers open for address `0x40`. Match each three-wire servo plug to the board markings: signal to the channel signal pin, positive to V+, and ground to GND. Do not connect servo power to ESP32 3.3 V or to PCA9685 VCC; VCC powers only the driver's logic. Verify plug polarity against the exact driver board before applying power.

On boot/reset, the code detects the driver, configures it for 50 Hz, and commands all five servos closed. During the mission, it only opens them; it does not close them again until reset. A missing PCA9685 or failed I2C command is logged at 115200 baud and prevents the mission from starting.

Motor direction flags in the source compensate for mirrored mounting. Check the wheel directions with the chassis raised before placing it on the field.
