# Competition program

`MinimalistCareBot.ino` is the one program uploaded for the competition run. After power-up, press the start button on GPIO32 once. The ESP32 then runs the complete route without further input.

The other sketches in this repository are for development and hardware testing. Uploading one of them replaces this program until `MinimalistCareBot.ino` is uploaded again.

The official robot photographs are in `../../images/`. They show a four-wheel chassis. The two motors on each side must act as one side pair, while the code continues to use one left command and one right command. Check that the motor driver can carry the combined current.

Use `WIRING.md` for the pin map and servo channels.
