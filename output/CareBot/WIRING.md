# Wiring labels

Target: common 30-pin ESP32-WROOM-32 DevKit, using the pin assignments in `CareBotESP32.ino`. The DOIT ESP32 DEVKIT V1 labels and header sides below assume the USB connector is at the bottom and the ESP32 module is at the top. Other 30-pin boards may print `IO25` or `25` where the DOIT board prints `D25`. Match the GPIO number and the actual board silkscreen before connecting wires. `TX2` is GPIO17 and `RX2` is GPIO16. GPIO34 and GPIO35 are input-only, which suits ECHO and the left IR output. The L298N and PCA9685 are separate driver boards. GPIO32 is unused.

| DOIT label | Other common labels | GPIO in code | DOIT header side | Connect to |
| --- | --- | ---: | --- | --- |
| `D25` | `IO25` or `25` | 25 | Left | L298N IN1 |
| `D26` | `IO26` or `26` | 26 | Left | L298N IN2 |
| `D27` | `IO27` or `27` | 27 | Left | L298N IN3 |
| `D14` | `IO14` or `14` | 14 | Left | L298N IN4 |
| `D33` | `IO33` or `33` | 33 | Left | L298N ENA, remove its jumper |
| `TX2` | `IO17` or `17` | 17 | Right | L298N ENB, remove its jumper |
| `D23` | `IO23` or `23` | 23 | Right | Ultrasonic TRIG |
| `D34` | `IO34` or `34` | 34 | Left | Ultrasonic ECHO through the divider below |
| `D35` | `IO35` or `35` | 35 | Left | Front-left IR digital output, at a compatible logic voltage |
| `RX2` | `IO16` or `16` | 16 | Right | Front-right IR digital output, at a compatible logic voltage |
| `D21` | `IO21` or `21` | 21 | Right | PCA9685 SDA |
| `D22` | `IO22` or `22` | 22 | Right | PCA9685 SCL |

The DevKit V1 `3V3` pin supplies PCA9685 VCC logic power. Use a `GND` pin for the common ground. `VIN` is not the servo rail; do not connect the 12 V motor battery to it. `EN` is the board reset/enable pin, not the L298N ENA or ENB input. The board-label mapping follows the [DevKit V1 board definition](https://github.com/wokwi/wokwi-boards/blob/main/boards/esp32-devkit-v1/board.json); clone silkscreens can differ.

## Motor driver and power

- OUT1/OUT2 connect to the left-side motor pair. OUT3/OUT4 connect to the right-side motor pair.
- Connect battery positive to the L298N 12 V motor-supply terminal. Connect battery negative to L298N GND.
- Remove the ENA and ENB jumpers. GPIO33 and GPIO17 supply PWM to those pins.
- Never connect 12 V to the module's 5 V terminal or directly to the ESP32. Follow the exact module's instructions for its 5 V regulator jumper.
- Confirm that the L298N module can handle the combined startup and stall current of two motors on each channel.
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

This gives approximately 3 V at GPIO34 for a 5 V ECHO output. Power an HC-SR04 according to its specification. Mount it so the robot structure and carried parts do not block its view of the wall.

## IR sensors

GPIO35 has no internal pull-up. An open-collector IR output may need an external pull-up to 3.3 V; a 5 V push-pull output needs level conversion. Mount both sensors side by side at the front and keep them close enough to the floor to distinguish black arena tape from the surrounding surface. The code confirms a marker only while both sensors report black.

## PCA9685 and servos

| Connection | Destination |
| --- | --- |
| ESP32 `D21` / GPIO21 | PCA9685 SDA |
| ESP32 `D22` / GPIO22 | PCA9685 SCL |
| ESP32 `3V3` | PCA9685 VCC logic power |
| Common ESP32 `GND` | PCA9685 GND and OE |
| Regulated servo supply positive | PCA9685 V+ |
| PCA9685 channel 0 | Bin A servo, 2 kits |
| PCA9685 channel 1 | Middle-bin servo, 6 kits |
| PCA9685 channel 2 | Bin B servo, 2 kits |
| PCA9685 channel 3 | First beam release servo |
| PCA9685 channel 4 | Second beam release servo |

Leave the PCA9685 address jumpers open for address `0x40`. Match each three-wire servo plug to the board markings: signal to the channel signal pin, positive to V+, and ground to GND. Do not connect servo power to ESP32 3.3 V or to PCA9685 VCC; VCC powers only the driver's logic. Verify plug polarity against the exact driver board before applying power.

On boot/reset, `CareBotESP32.ino` detects the driver, configures it for 50 Hz, commands channels 0 through 4 to their rest positions, performs its sensor checks, and starts the route automatically. Channels 0 and 2 move from 90 degrees down to 20 degrees for the two 2-bin mechanisms. Channel 1 moves from 90 degrees up to 160 degrees for the 6-bin mechanism. Confirm the actual servo socket numbers and safe angles before loading the bins. A missing PCA9685 or failed I2C command prevents movement.

Motor direction flags in the source compensate for mirrored mounting. Check the wheel directions with the chassis raised before placing it on the field.
