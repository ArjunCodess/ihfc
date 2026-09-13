# Wiring sheet

This pin map is for a DOIT ESP32 DEVKIT V1 and DRV8833. Use the GPIO numbers printed on the board, not the physical pin positions. GPIO34 and GPIO35 are input-only pins, which is correct here because they receive the ultrasonic ECHO and IR signals.

| ESP32 GPIO | Connect to |
| ---: | --- |
| 25 | DRV8833 AIN1 |
| 26 | DRV8833 AIN2 |
| 27 | DRV8833 BIN1 |
| 14 | DRV8833 BIN2 |
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

- AOUT1/AOUT2 connect to the left motor. BOUT1/BOUT2 connect to the right motor.
- The motors are rated 12 V, but DRV8833 VM has a 2.7–10.8 V operating range. Never connect a 12 V supply to this driver's VM pin. Replace the driver for 12 V operation, or use no more than 10.8 V and accept reduced motor speed. The replacement must also handle the measured stall current.
- Hold nSLEEP high at 3.3 V if the breakout does not already do so. Check its exact pin labels.
- Supply all five servos from a separate suitable regulated rail. The regulator must handle their combined demands; its current rating cannot be finalized until the servo models and loads are known.
- Power the ESP32 DevKit through its supported USB or regulated board-power input. Follow that board's requirements; do not connect a raw motor battery to its 3.3 V pin.
- Join the ESP32, sensors, servo supply and motor-driver grounds.
- If the driver board does not provide them, add pull-downs to its four control inputs so the motor inputs remain low during reset.

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
