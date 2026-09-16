# CareBot hardware test

`CareBotHardwareTest.ino` is a separate diagnostic sketch for the 30-pin ESP32 NodeMCU board with USB-C and an ESP32-WROOM-32 module. In Arduino IDE, select **ESP32 Dev Module**, install **Adafruit PWM Servo Driver Library** and **Adafruit BusIO**, and choose the USB serial port. The sketch uses GPIO numbers, so read the `IO` or GPIO markings on your actual board rather than assuming a particular header position. The pin assignments match the [CareBot wiring sheet](../CareBot/WIRING.md).

Uploading this file replaces the mission firmware until you upload `CareBotESP32.ino` again. Disconnect or unload the kit and beam mechanisms and raise the drive wheels **before powering or resetting the board**. Open Serial Monitor at **115200 baud** to observe the logs. The test starts by itself after boot: all 16 PCA9685 channels are swept one at a time, then the left and right motors each run forward and reverse. The motors stay off after the test.

The sketch logs ultrasonic echo time and distance, both IR raw levels and `BLACK`/`CLEAR` states, the START input, I²C scan results, PCA9685 acknowledgment and prescale, planned channels 0–4 PWM register values, motor command pins and PWM duty, chip model, CPU speed, flash size, and free heap. Sensor and driver values repeat every two seconds after the automatic test; each motor burst also logs its active PWM and direction pins.

No command or START press is needed for the automatic test. Optional serial commands are:

| Command | Result |
| --- | --- |
| `?` | Show commands |
| `b` | Board values |
| `i` | Scan I²C addresses |
| `p` | PCA9685 and channels 0–4 register values |
| `u` | One ultrasonic echo and distance |
| `r` | Both front IR inputs |
| `0`–`9`, `A`–`F` | Move one PCA9685 channel; `A` is 10 and `F` is 15 |
| `t` | Move all 16 channels one by one to locate connected servos |
| `T` | Rerun the automatic servo-and-motor test |
| `!` then `L` | Arm and briefly test the left motor forward and reverse |
| `!` then `R` | Arm and briefly test the right motor forward and reverse |
| `x` | Stop motors, disarm motor tests, or interrupt an active test |

The servo test uses 225 and 375 PCA9685 ticks at 50 Hz, about 1.1 and 1.8 ms. Each motor direction is commanded at PWM 145 for 300 ms, with the motors off between directions and after the test. The complete actuator test takes about 32 seconds, and motor motion begins only after the servo sweep finishes. Watch the two connected servos and the raised wheels to confirm real movement. An I²C acknowledgment or PWM register readback only proves the driver accepted commands. This wiring has no measurement input for servo supply voltage, battery voltage, motor current, wheel speed, OE level, or servo position; the log marks those values `unmeasured`.

At idle, `off_register=4096 output_state=FULL_OFF` is a normal PCA9685 full-off value. It does not report whether a servo is attached. A servo test command changes that channel to a pulse value, and the `SERVO` line reports the commanded ticks and register readback. Likewise, `left_pwm_gpio33=0` and `right_pwm_gpio17=0` mean the diagnostic has disabled the motors. For IR sensors, put each sensor over clear floor and then over black tape; its raw level and `BLACK`/`CLEAR` label should change. A constant `BLACK` may reflect the surface, inversion, or wiring, so the single idle reading is not enough to diagnose it.
