# CareBot showcase pack

CareBot carries ten preloaded medical kits in three bins and two beams in servo grippers. It follows a fixed route, drops the kits in groups of 2, 6 and 2, then opens both beam grippers at its final stop. Every delivery servo stays open. The robot does not reverse after dropping the beams.

## What to open

| File | Use it for |
| --- | --- |
| `SHOWCASE.html` | Open in any browser for the audience-facing overview and route diagram. Works offline and can be printed. |
| `CareBotESP32/CareBotESP32.ino` | Open in Arduino IDE to show or upload the actual program. Keep its folder name unchanged. |
| `COMPONENTS.md` | Explain the component quantities and what each part does. |
| `WIRING.md` | Connect the electronics using the exact GPIO assignments in the sketch. |
| `CODE-EXPLAINED.md` | Explain how sensing, movement and the five servos work. |
| `DEMO-GUIDE.md` | Prepare the live run and read a short speaking script. |
| `CALIBRATION.md` | Record the measured settings before running on the field. |
| `VERIFICATION.md` | Show what has actually been tested and what is still unverified. |
| `TECHNICAL-GUIDE.md` | Read the longer build and calibration notes. |
| `tests/` and `evidence/` | Run the host regression tests or show saved compile and test results. |
| `compile.ps1` | Compile with an already-installed Arduino CLI and ESP32 core. |
| `SHA256SUMS.txt` | Check that the packaged files have not changed. |

## Software setup

Use a **DOIT ESP32 DEVKIT V1**. In Arduino IDE, install **esp32 by Espressif Systems**, version **3.3.5** to reproduce the verified build, and select **DOIT ESP32 DEVKIT V1** under Tools > Board. Choose the connected board's port. The separate **Arduino ESP32 Boards** package is not the package used for this build. No additional servo library is needed.

Open `CareBotESP32/CareBotESP32.ino`, compile, then upload only after checking the pin wiring and calibration. Set Serial Monitor to **115200 baud**. Install the board package and any USB serial driver before leaving for a venue without internet. The ZIP does not include Arduino IDE, board packages, USB drivers or a C++ compiler.

Release START after powering up, load the robot after the servos close, then press START or send `s`. Sending `x` while connected stops the program. Use the main battery switch when running without a computer. Reset closes the servos and allows another run; clear the mechanisms before resetting.

## What is ready

The packaged source passed an actual ESP32 compilation and host regression tests. It has not been tested on a physical robot. Motor electrical specifications, servo sizing, movement timings, landing positions and the battery setup still need checking. The source contains example calibration values; a successful compile does not make those values correct for your chassis.

Only the current DRV8833, open-only servo version is included. The development workspace's old test sketch and older L298N variants are not part of this pack.
