# CareBot project pack

CareBot carries ten preloaded medical kits in three bins and two beams in servo grippers. It follows a fixed route, drops the kits in groups of 2, 6 and 2, then opens both beam grippers at its final stop. Every delivery servo stays open. The robot does not reverse after dropping the beams.

## What to open

| File | Use it for |
| --- | --- |
| `SHOWCASE.html` | Open in any browser for the audience-facing overview and route diagram. Works offline and can be printed. |
| `CareBotESP32/CareBotESP32.ino` | Development and fault-checking sketch. Keep its folder name unchanged. |
| `COMPONENTS.md` | Explain the component quantities and what each part does. |
| `WIRING.md` | Connect the electronics using the exact GPIO assignments in the sketch. |
| `CODE-EXPLAINED.md` | Explain how sensing, movement and the five servos work. |
| `DEMO-GUIDE.md` | Prepare the live run and read a short speaking script. |
| `CALIBRATION.md` | Record the measured settings before running on the field. |
| `VERIFICATION.md` | Show what has actually been tested and what is still unverified. |
| `TECHNICAL-GUIDE.md` | Read the longer build and calibration notes. |
| `tests/` and `evidence/` | Run the host regression tests or show saved compile and test results. |
| `compile.ps1` | Compile with Arduino CLI, including the copy bundled with Arduino IDE, and the ESP32 core. |
| `SHA256SUMS.txt` | Check that the project-pack files have not changed. |

## Software setup

Use a common **30-pin ESP32-WROOM-32 DevKit** and a PCA9685 servo driver at address `0x40`. In Arduino IDE, install **esp32 by Espressif Systems** and select **ESP32 Dev Module** or your board's matching 30-pin DevKit entry. Choose the connected board's port. Install **Adafruit PWM Servo Driver Library** and its **Adafruit BusIO** dependency through Library Manager. The sketch uses `Wire` on GPIO21 and GPIO22.

This page covers the development sketch in this folder. The competition upload is `../MinimalistCareBot/MinimalistCareBot.ino`.

Open `CareBotESP32/CareBotESP32.ino`, compile, then upload only after checking `WIRING.md` and calibration. Set Serial Monitor to **115200 baud**. With empty mechanisms, send `0` through `9` or `A` through `F` to test one PCA9685 socket without driving the motors. `s` starts the mission and `x` stops it. Install the board package, Adafruit libraries and any USB serial driver before leaving for a venue without internet. `output/CareBot.zip` is a copy of this project pack; it does not include Arduino IDE, board packages, USB drivers or a C++ compiler.

Release START after powering up, load the robot after the servos close, then press START or send `s`. Sending `x` while connected stops the program. Use the main battery switch when running without a computer. Reset closes the servos and allows another run; clear the mechanisms before resetting.

## What is ready

The current source passed an actual ESP32 compilation. The host regression log belongs to the previous revision; the current host suite has not been rerun because `g++` is unavailable in this Windows session. The code has not been tested on a physical robot. Motor electrical specifications, servo sizing, movement timings, landing positions and the battery setup still need checking. The source contains example calibration values; a successful compile does not make those values correct for your chassis.

Only the current L298N motor-driver and PCA9685 servo-driver version is included. Older driver variants are not part of this pack.
