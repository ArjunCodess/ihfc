# Components and quantities

| Part | Quantity | Job / selection status |
| --- | ---: | --- |
| DOIT ESP32 DEVKIT V1 | 1 | Main controller and exact Arduino board profile used by the sketch. |
| DRV8833 dual motor-driver module | 1 | Cannot use a 12 V VM supply; replace it for rated-voltage operation. |
| Dual-shaft 12 V DC geared motor, 500 RPM | 2 | Existing stock; stall current still needs to be identified. |
| Front ultrasonic sensor | 1 | Measures the distance to the wall. Wiring assumes an HC-SR04-type TRIG/ECHO interface. |
| Rear IR reflectance sensor, digital output | 1 | Points downward and detects transverse black markers. Output must be ESP32-compatible. |
| Bin flap positional servo | 3 | Opens bins carrying 2, 6 and 2 kits; exact servo models depend on flap load. |
| Beam gripper positional servo | 2 | Opens the front and right grippers; exact servo models depend on grip force. |
| Start pushbutton | 1 | Starts the programmed route. No separate stop pushbutton is used. |
| Main battery power switch | 1 | Disconnects power independently of the program. |
| Drive wheel | 2 | Match the wheel hub to the motor shaft. |
| Caster | 1 | Supports the remaining end of the chassis. |
| Motor mount | 2 | Fixes the two motors to the chassis. |
| Chassis | 1 | Supports the electronics, mechanisms and payload. |
| Bin with flap | 3 | Separate preloaded gravity-release groups. |
| Gripper mechanism | 2 | One across the front, one along the right side; bought or printed. |
| Motor-compatible battery pack | 1 | Voltage and current capability remain to be selected. |
| Matching battery charger | 1 | Must match the chosen pack chemistry and cell count. |
| Regulated power supplies | As required | Supply the ESP32 board and five servos at their specified voltages. Do not feed servos through the ESP32 board. |
| 10 kΩ and 15 kΩ resistors | 1 each | Voltage divider on a 5 V ultrasonic ECHO signal. |
| Pull-down / pull-up resistors | As required | Depend on what the motor-driver and IR modules already provide. |
| USB data cable | 1 | Uploads code and connects Serial Monitor. |
| Connectors, wire, screws and servo horns | As required | Electrical and mechanical assembly. |

The payload is **10 kits plus 2 beams**. Total actuators are **2 DC motors and 5 servos**. A gripper kit with an included servo supplies one of those five servos; it does not add a sixth or seventh. A dual-shaft motor is still one motor, not two independently controlled outputs.

The Amazon gripper package contents and the exact printable model's dimensions were not verified. Confirm jaw clearance, servo fit and holding force with an actual beam before making or buying both units.
