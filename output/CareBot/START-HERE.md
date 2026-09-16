# CareBot project pack

CareBot carries ten preloaded medical kits in three bins and two beams in servo holders. It follows a fixed route, drops the kits in groups of 2, 6 and 2, then releases the two beams after separate left turns. Every delivery servo stays open.

## What to open

| File | Use it for |
| --- | --- |
| `SHOWCASE.html` | Open in any browser for the audience-facing overview and route diagram. Works offline and can be printed. |
| `CareBotESP32/CareBotESP32.ino` | Robot program to compile and upload. Keep its folder name unchanged. |
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

Upload `CareBotESP32/CareBotESP32.ino` and use this folder's `WIRING.md`.

Open `CareBotESP32/CareBotESP32.ino`, compile, then upload after checking `WIRING.md` and calibration. Set Serial Monitor to **115200 baud** to view motor, ultrasonic, IR, and servo logs. Serial `x` stops an active route. Install the board package, Adafruit libraries and any USB serial driver before leaving for a venue without internet. `output/CareBot.zip` is a copy of this project pack; it does not include Arduino IDE, board packages, USB drivers or a C++ compiler.

Connecting the battery starts the route automatically after initialization. Keep the drive wheels raised and the mechanisms empty during the first powered check. Sending `x` while connected stops the program. The main battery switch provides the physical stop. Reset returns the servos to their rest positions and immediately starts another run.

## What is ready

The current source passed an actual ESP32 compilation. The host regression log belongs to the previous revision; the current host suite has not been rerun because `g++` is unavailable in this Windows session. The code has not been tested on a physical robot. Motor electrical specifications, servo sizing, movement timings, landing positions and the battery setup still need checking. The source contains example calibration values; a successful compile does not make those values correct for your chassis.

Only the current L298N motor-driver and PCA9685 servo-driver version is included. Older driver variants are not part of this pack.
