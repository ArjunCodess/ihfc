# Components and quantities

| Part | Quantity | Job / selection status |
| --- | ---: | --- |
| DOIT ESP32 DEVKIT V1 | 1 | Main controller and exact Arduino board profile used by the sketch. |
| L298N dual motor-driver module | 1 | Drives the left and right motor pairs; verify combined stall-current capacity for each channel. |
| 12 V DC geared motor, 500 RPM | 4 | One motor at each wheel in the official photographs. Two motors act together on each side. |
| Front ultrasonic sensor | 1 | Measures the distance to the wall. Wiring assumes an HC-SR04-type TRIG/ECHO interface. |
| Front IR reflectance sensor, digital output | 2 | Point downward at the front and confirm black arena tape together. Outputs must be ESP32-compatible. |
| PCA9685 16-channel servo driver | 1 | Drives all five servos at 50 Hz over I2C; default address `0x40`. |
| Bin flap positional servo | 3 | Opens bins carrying 2, 6 and 2 kits; exact servo models depend on flap load. |
| Beam release positional servo | 2 | Opens the first and second beam holders; exact servo models depend on grip force. |
| Main battery power switch | 1 | Disconnects power independently of the program. |
| Drive wheel | 4 | Four wheels are visible in the official photographs. |
| Motor mount | 4 | Fixes the four motors to the chassis. |
| Chassis | 1 | Supports the electronics, mechanisms and payload. |
| Bin with flap | 3 | Separate preloaded gravity-release groups. |
| Beam holder / release mechanism | 2 | Holds and releases the first and second containment beams. |
| Motor-compatible battery pack | 1 | Voltage and current capability remain to be selected. |
| Matching battery charger | 1 | Must match the chosen pack chemistry and cell count. |
| Regulated power supplies | As required | Supply the ESP32 board and PCA9685 V+ servo rail at their specified voltages. Do not feed servos through the ESP32 board or PCA9685 VCC. |
| 10 kΩ and 15 kΩ resistors | 1 each | Voltage divider on a 5 V ultrasonic ECHO signal. |
| Pull-down / pull-up resistors | As required | Depend on what the motor-driver and IR modules already provide. |
| USB data cable | 1 | Uploads code and connects Serial Monitor. |
| Connectors, wire, screws and servo horns | As required | Electrical and mechanical assembly. |

The payload is **10 kits plus 2 beams**. Total actuators are **4 DC motors and 5 servos**. The code still has two drive outputs because the two motors on each side receive the same direction and speed command. Confirm that the driver and wiring can safely carry both motors on one channel.

The Amazon gripper package contents and the exact printable model's dimensions were not verified. Confirm jaw clearance, servo fit and holding force with an actual beam before making or buying both units.
