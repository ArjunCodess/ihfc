# CareBot and SealBot: simple locked architecture

Revised concept | 7 September 2026 | All dimensions in mm unless stated

This is the team's architecture, kept simple. CareBot picks up a kit or coloured piece, lifts it to a colour sensor, drops it into the correct tilted box, and later empties that box with a servo gate. SealBot has two beam grippers at the lower front, with the movable input flap directly above them. It places the beams first, then uses that flap and a small belt-and-tray conveyor to collect and release the discs. CareBot uses one Uno R3, one PCA9685, and one ESP32-CAM. SealBot uses one Uno R3 by itself.

## 1. Complete architecture

| Robot | Front | Inside | Rear |
|---|---|---|---|
| CareBot | One V-shaped pickup mouth and two rubber lift belts | Colour sensor, one servo sorting flap, tilted storage boxes | One simple servo gate on each box |
| SealBot | Two beam grippers at the bottom, JCB input flap directly above them | One rubber-belt conveyor with three disc trays | Fixed output flap with one servo gate and a line sensor |

The Uno R3 is the main controller on both robots. CareBot's PCA9685 produces the nine servo signals, which leaves the Uno free for the motors and sensors. Its ESP32-CAM finds targets and sends short commands such as LEFT, RIGHT, CENTRE, and TARGET FOUND to the Uno. SealBot does not need a camera for the fixed-layout Junior field; its Uno follows the line and runs its four servos directly.

### Size lock

| Robot | Maximum build envelope | What is included |
|---|---|---|
| CareBot | 230 W x 240 L x 280 H | Wheels, lift, camera, seven tilted boxes, gates, and servo horns |
| SealBot | 200 W x 260 L x 210 H | Wheels, front grippers, raised input flap, conveyor, rear outlet, and servo horns |
| Both in the start zone | 440 W x 260 L | Includes a 10 mm gap between the robots |

The rulebook starting zone is 480 x 280 mm, so the pair has 40 mm of spare width and 20 mm of spare length. The loaded beams must lie along the 280 mm direction and must be checked in a physical 480 x 280 mm gauge before the final frame is cut.

## 2. CareBot

### Simple pickup

Use two vertical rubber belts facing each other at the front. Each belt runs around two ordinary pulleys. One N20 geared motor turns both belts inward and upward. A V-shaped mouth guides a kit or coloured piece between them, and the belts carry it to the top.

Use wide rubber bands, timing belts, or rubber strips joined into loops. One belt mount can move slightly against a spring so both piece sizes fit. This is the simple replacement for caterpillar wheels.

### Sensor and boxes

At the top, the piece stops in front of a TCS34725 colour sensor. Directly below the sensor, one servo flap points toward the required storage box. A second servo opens the small floor under the piece, so it falls into that box.

Use seven tilted boxes because the game pieces have seven final groups:

- Three kit boxes hold 6 for H, 2 for PCC-A, and 2 for PCC-B.
- Four coloured-piece boxes hold red for H, green for RZ, yellow for PCC-A, and yellow for PCC-B.

The kits can use a simple count. The first six go into the H box, the next two into PCC-A, and the final two into PCC-B. The sensor sorts the coloured pieces. Every box is already tilted toward its outlet and has one SG90 servo gate.

### CareBot sequence

1. **Pick up one piece.** The camera centres the front mouth, then the rubber belts pull the piece upward.
2. **Read it.** The lift stops at the colour sensor.
3. **Select its box.** One servo points the sorting flap toward the correct box.
4. **Store it.** The floor servo opens and the piece falls into the tilted box.
5. **Dump the group.** At the destination, the camera or line sensor confirms position and the box gate opens. Gravity drops the complete group into the area.

That is the whole CareBot mechanism. There is one lift, one sensor point, one sorting flap, and tilted boxes with gates.

## 3. SealBot

### Exact front layout

The two grippers sit at the lowest front portion of the robot. They hold the two wooden beams standing on the field. The movable input flap sits directly above the grippers. The output flap stays fixed at the rear.

The input flap remains raised during the complete beam task, so it does not hit the beams or block the grippers.

### Beam sequence

1. Both beams start standing in the two front grippers.
2. SealBot drives to the first beam area and opens the first gripper.
3. SealBot drives to the second beam area and opens the second gripper.
4. Both beams remain standing in their areas. The beam task is finished.
5. Only now can the input flap move down.

There is no later beam push, no staging step, and no beam mechanism inside the robot.

### Disc input flap

The input flap moves like a small JCB bucket. A metal-gear servo lowers it to the field, the robot drives forward until a disc sits on the flap, and the servo raises the flap. At the top, the flap tips the disc onto the first tray inside the robot.

### Simple conveyor without caterpillar wheels

Use one homemade loop conveyor. Two rubber belts or timing belts run around a front shaft and a rear shaft. Attach three small flat trays across the belts. One N20 geared motor turns the rear shaft.

After the flap loads a disc, the motor moves the belt forward by one tray position. The next empty tray arrives below the input flap, while the loaded disc moves toward the rear. A small IR sensor or microswitch marks each stopping position.

This gives the same action as the proposed caterpillar wheel and tray, but it only needs ordinary pulleys, rubber belts, a motor, and three flat trays.

### Rear output

The rear output flap stays in one place. Connect only the left, centre, and right outputs of a downward five-channel line-sensor board. The Uno follows the line to the required mark, then a wheel-encoder distance or a separate reflective marker gives the final stop.

When the correct position is detected, one SG90 opens the output gate and one disc drops. The gate closes, then the conveyor moves forward by one tray position. Repeat this at the next two positions.

## 4. SealBot sequence

1. **Carry the beams.** Keep the input flap raised and hold both beams in the two lower front grippers.
2. **Drop the beams.** Open each gripper at its area. The beam task ends here.
3. **Enable the flap.** Lower the input flap only after both grippers are empty.
4. **Collect a disc.** Scoop it with the flap and tip it onto the front tray.
5. **Move the tray.** Advance the rubber-belt conveyor by one position.
6. **Reach an output point.** Follow the field line with the left, centre, and right sensor outputs, then stop at the marker or stored encoder count.
7. **Drop one disc.** Open the fixed rear output gate, close it, then advance the next tray.

## 5. Simple split control

CareBot uses an Uno R3 with one PCA9685 because it has nine servos. SealBot has only four servos, so its Uno R3 drives them directly and needs no expansion board.

| Part | Job |
|---|---|
| CareBot Uno R3 | Runs the sequence, three DC motors, colour sensor, switches, and PCA9685 |
| CareBot PCA9685 | Produces all nine CareBot servo signals over the two-wire I2C connection |
| CareBot ESP32-CAM | Finds objects and alignment marks, then sends a short result to the Uno |
| SealBot Uno R3 | Runs three DC motors, four servos, three line-sensor channels, start input, and conveyor index sensor |
| Two DRV8833 boards | One drives the left and right wheels; the other drives the lift or conveyor motor |
| TCS34725 | Reads the CareBot piece colour at the top of the lift |
| Five-channel line sensor | SealBot uses only its left, centre, and right outputs for simple line following |

The ESP32-CAM remains CareBot's main vision sensor, but it only sends horizontal error, target type, and target visible or lost. The CareBot Uno handles real-time movement. The PCA9685 and TCS34725 share the same A4/A5 I2C connection.

SealBot's pin budget fits one Uno without making the wiring awkward. Use D2-D7 for the three DC motors, D8-D11 for the four servos, A0-A2 for the three line signals, A3 for start, and A4 for the conveyor index sensor. D0/D1 remain free for USB serial, and A5 remains spare. The emergency stop cuts actuator power directly and does not need a software pin.

### Why the earlier design had many boards

The ESP32-CAM uses most of its pins for the camera, so it cannot control the complete robot reliably by itself. CareBot still needs one PCA9685 because nine servo signals would crowd the Uno. SealBot needs neither a PCA9685 nor a PCF8575 because four direct servo connections and three line signals fit its Uno.

No separate USB-UART programmer is purchased. During setup, hold the CareBot Uno's ATmega328P in reset and use the Uno's onboard USB-to-serial interface to upload the ESP32-CAM firmware. Add a two-resistor divider between the Uno TX and the 3.3 V ESP32-CAM RX. After programming, reconnect the camera to the Uno using two software-serial pins.

### Power kept simple

Use two regulators on each robot. One LM2596 provides clean 5 V power for the Uno and, on CareBot, the camera. One XL4015 provides the actuator rail for the servos and motors. Connect all grounds at one common point, and never power the servos from the Uno's 5 V pin. The program moves only the required gate or flap, so one 5 A actuator regulator is enough per robot.

## 6. Actuator count

| Robot | DC motors | Servos |
|---|---|---|
| CareBot | 2 drive motors and 1 lift motor | 1 sensor-floor servo, 1 sorting-flap servo, and 7 box-gate servos |
| SealBot | 2 drive motors and 1 conveyor motor | 2 beam-gripper servos, 1 JCB-flap servo, and 1 output-gate servo |

The complete build uses thirteen working servos. Buy three spare SG90 servos and one spare MG90S because small servos are easy to damage during testing.

## 7. Complete parts and pricing

These are planning prices checked on 7 September 2026. Listed prices can change, and stock status must be checked again before payment.

### Supplier comparison for the main parts

| Part | Quantity | Robocraze Rs | Quartz Rs | Robu Rs |
|---|---|---|---|---|
| ESP32-CAM with camera | 1 | 689 | 675 | 739 |
| Uno R3 CH340 compatible board | 2 | 281 | 218 | 229 |
| PCA9685 16-channel servo driver | 1 | 255 | 212 | 219 |
| DRV8833 motor-driver board | 4 | 67 | 63, out of stock | 399 current listing |
| 2WD chassis kit | 2 | 285 | 265 | 428 |
| SG90 servo | 13 | 91 | 86 | 93 |
| MG90S servo | 4 | 149, out of stock | 124 | 125 |
| TCS34725 colour sensor | 1 | 306 | 191, out of stock | 269 |
| Five-channel line sensor | 1 | 189 | 160 | 177 listing |
| LM2596 regulator | 2 | 48 | 107 with display | 41 |
| XL4015 regulator | 2 | 84 | 79 | 71 |
| B3 balance charger | 1 | 329 | 294 | 336 |
| 7.4 V 2200 mAh high-discharge LiPo | 2 | No close match found | 1,221, out of stock | 1,399 |

Quartz currently has the lowest Uno R3 and PCA9685 prices and lists both in stock. The chosen cart still mixes suppliers because Robocraze is cheaper for the DRV8833 and Robu has the selected colour sensor, geared motors, regulators, and high-discharge batteries. The low-cost Rs 366 Quartz lithium-ion pack is not used because its stated maximum discharge current is only 2 A.

### Electronics and motion

| Part | Quantity | Unit Rs | Subtotal Rs | Use |
|---|---|---|---|---|
| [Uno R3 CH340 compatible board](https://quartzcomponents.com/products/arduino-uno-1) | 2 | 218 | 436 | One main controller per robot |
| [PCA9685 16-channel servo driver](https://quartzcomponents.com/products/16-channel-12-bit-pwm-servo-driver-i2c-interface-pca9685-for-arduino-raspberry-pi) | 1 | 212 | 212 | CareBot's nine servo signals |
| [ESP32-CAM with camera](https://quartzcomponents.com/collections/wireless/products/esp32-camera-development-board-wifi-bluetooth-with-ov2640-camera-module) | 1 | 675 | 675 | CareBot vision only |
| [DRV8833 motor-driver board](https://robocraze.com/products/drv8833-2-channel-dc-motor-driver) | 4 | 67 | 268 | Two per robot |
| [2WD chassis kit with motors and wheels](https://quartzcomponents.com/collections/robotics/products/2wd-smart-robot-car-chassis-kit) | 2 | 265 | 530 | One donor base per robot |
| [N20 6 V 100 RPM geared motor](https://robu.in/product-category/micro-gear-motor-without-encoder/) | 2 | 289 | 578 | CareBot lift and SealBot conveyor |
| [SG90 servo](https://quartzcomponents.com/products/tower-pro-sg90-servo-9-gms-mini-micro-servo-motor) | 13 | 86 | 1,118 | Ten working and three spare |
| [MG90S metal-gear servo](https://quartzcomponents.com/products/tower-pro-mg90s-servo-motor-with-metal-gear) | 4 | 124 | 496 | Three working and one spare |
| Electronics and motion subtotal | | | 4,313 | |

### Sensors and controls

| Part | Quantity | Unit Rs | Subtotal Rs | Use |
|---|---|---|---|---|
| [TCS34725 colour sensor](https://robu.in/product-category/light-color-sensor/) | 1 | 269 | 269 | CareBot sensor point |
| [Five-channel line sensor](https://quartzcomponents.com/products/tcrt5000l-5-channel-tracking-sensor-module-infrared-sensor) | 1 | 160 | 160 | SealBot uses three outputs |
| Wheel encoder sensor | 4 | 60 | 240 | Two per robot |
| IR or reflective position sensor | 6 | 50 | 300 | Lift, flap, conveyor, and output checks |
| Microswitch | 10 | 20 | 200 | Grippers, gates, and end stops |
| Start pushbutton | 2 | 25 | 50 | One per robot |
| Emergency-stop switch | 2 | 150 | 300 | One per robot |
| Sensors and controls subtotal | | | 1,519 | |

### Power and wiring

| Part | Quantity | Unit Rs | Subtotal Rs | Use |
|---|---|---|---|---|
| [7.4 V 2200 mAh 2S LiPo battery](https://robu.in/product-category/2-cell-lipo-7-4v-8-4v/) | 2 | 1,399 | 2,798 | One per robot |
| [2S/3S balance charger](https://quartzcomponents.com/products/b3-lithium-battery-charger-for-2s-and-3s-lipo-batteries) | 1 | 294 | 294 | Shared charger |
| LiPo-safe bag | 1 | 350 | 350 | Battery storage |
| [LM2596 regulator](https://robu.in/product-category/buck-converter/) | 2 | 41 | 82 | Logic and camera power |
| [XL4015 5 A regulator](https://robu.in/product-category/buck-converter/) | 2 | 71 | 142 | One actuator rail per robot |
| Fuses and holders | 2 sets | 75 | 150 | One set per robot |
| Capacitors and motor-noise parts | 1 lot | 250 | 250 | Both robots |
| Wire, connectors, perfboard, and standoffs | 1 lot | 900 | 900 | Both robots |
| Power and wiring subtotal | | | 4,966 | |

### Body and mechanisms

| Material | Allowance Rs | Use |
|---|---|---|
| Plywood, acrylic, and PET sheet | 900 | Boxes, trays, flaps, decks, and covers |
| Screws, nuts, washers, and spacers | 600 | Structure |
| Rods, hinges, springs, and linkage wire | 450 | Flap and grippers |
| Rubber belts, pulley material, foam, and rubber strips | 500 | CareBot lift and SealBot conveyor |
| Chute liners and flexible outlet pieces | 250 | Tilted boxes and rear output |
| Cutting, drilling, or small 3D-printed parts | 700 | Brackets and pulleys |
| Practice pieces and field mock-up | 600 | Testing |
| Labels and decoration | 250 | Final presentation |
| Body and mechanism subtotal | 4,250 | |

### Total

| Group | Rs |
|---|---|
| Electronics and motion | 4,313 |
| Sensors and controls | 1,519 |
| Power and wiring | 4,966 |
| Body and mechanisms | 4,250 |
| Shipping allowance | 900 |
| Repair reserve | 800 |
| Complete planning total | 16,748 |

Plan on Rs 16,750 if everything is bought new. If the school already has sheet material, wire, fasteners, and workshop access, expect about Rs 12,500 to Rs 13,000. This total already assumes that SealBot has no camera.

## 8. What to build next

1. Build one CareBot pickup using four pulleys, two rubber loops, and one N20 motor. Test it with the real kit and coloured piece.
2. Build one SealBot conveyor using two belt loops, two shafts, and three trays. Make it move exactly one tray per command.
3. Make the SealBot front from cardboard. Put the two grippers at the bottom and the raised input flap directly above them.
4. Wire the SealBot Uno to four servos, three line-sensor outputs, and the two DRV8833 boards. Test line following and one-tray indexing before building the final harness.
5. On CareBot, test the Uno, PCA9685, and three servos first, then add the colour sensor and camera message link. Buy the full set only after both mechanism tests work. A small proof set costs about Rs 3,500 to Rs 4,000.

## 9. Sources

- [Espressif ESP32-CAM camera pin map](https://github.com/espressif/esp32-camera/blob/master/examples/camera_example/main/camera_pinout.h)
- [Arduino Uno R3 hardware specification](https://docs.arduino.cc/hardware/uno-rev3)
- [NXP PCA9685 product page](https://www.nxp.com/products/power-drivers/lighting-driver-and-controller-ics/led-drivers/16-channel-12-bit-pwm-fm-plus-ic-bus-led-driver:PCA9685)
- [Texas Instruments DRV8833 product page](https://www.ti.com/product/DRV8833)
- [Robocraze component listings](https://robocraze.com/)
- [Quartz Components listings](https://quartzcomponents.com/)
- [Robu component listings](https://robu.in/)
