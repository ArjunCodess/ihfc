# Wiring sheet

This pin map is for a DOIT ESP32 DEVKIT V1 and L298N. Use the GPIO numbers printed on the board, not the physical pin positions. GPIO34 and GPIO35 are input-only pins, which is correct here because they receive the ultrasonic ECHO and IR signals.

| ESP32 GPIO | Connect to |
| ---: | --- |
| 25 | L298N IN1 |
| 26 | L298N IN2 |
| 27 | L298N IN3 |
| 14 | L298N IN4 |
| 16 | L298N ENA, remove its jumper |
| 17 | L298N ENB, remove its jumper |
| 23 | Ultrasonic TRIG |
| 34 | Ultrasonic ECHO through the divider below |
| 35 | Rear IR sensor digital output, at a compatible logic voltage |
| 18 | Bin A servo signal, 2 kits |
| 19 | Middle-bin servo signal, 6 kits |
| 21 | Bin B servo signal, 2 kits |
| 22 | Right beam gripper servo signal |
| 13 | Front beam gripper servo signal |
| 32 | Start button; other terminal to GND |

## Motor driver and power

- OUT1/OUT2 connect to the left motor. OUT3/OUT4 connect to the right motor.
- Connect battery positive to the L298N 12 V motor-supply terminal. Connect battery negative to L298N GND.
- Remove the ENA and ENB jumpers. GPIO16 and GPIO17 supply PWM to those pins.
- Never connect 12 V to the module's 5 V terminal or directly to the ESP32. Follow the exact module's instructions for its 5 V regulator jumper.
- Confirm that the L298N module can handle each motor's startup and stall current.
- Supply all five servos from a separate suitable regulated rail. The regulator must handle their combined demands; its current rating cannot be finalized until the servo models and loads are known.
- Power the ESP32 DevKit through its supported USB or regulated board-power input. Follow that board's requirements; do not connect a raw motor battery to its 3.3 V pin.
- Join the ESP32, sensors, servo supply and motor-driver grounds.

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

## IR and servos

GPIO35 has no internal pull-up. An open-collector IR output may need an external pull-up to 3.3 V; a 5 V push-pull output needs level conversion. Keep the sensor close enough to the floor to distinguish tape from the surrounding surface.

Every servo has three connections: signal to its assigned GPIO, power to the servo supply, and ground to the common ground. On boot/reset, the code commands all five servos closed. During the mission, it only opens them; it does not close them again until reset.

Motor direction flags in the source compensate for mirrored mounting. Check the wheel directions with the chassis raised before placing it on the field.
