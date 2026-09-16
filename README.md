# IHFC robot

The robot program is `output/CareBot/CareBotESP32/CareBotESP32.ino`. Connecting the battery starts one autonomous 2, 6, 2 kit route followed by the beam releases. Runtime logs report the ultrasonic sensor, both front IR sensors, every servo command, and motor commands at 115200 baud.

Use `output/CareBot/WIRING.md` for the ESP32 and PCA9685 connections. The current submission document is `output/pdf/Design_Details_Document_ST-77.pdf`, with its LaTeX source beside it. The photographs in `images/` are the official record of the built robot. See `docs/MODEL-CONSISTENCY.md` for differences found between the model, older notes and the current build. The source repository is [ArjunCodess/ihfc](https://github.com/ArjunCodess/ihfc).

The second robot has not been built. Its retained concept is in `docs/SECOND-ROBOT-FUTURE.md`.

The competition rulebook remains at `Rulebook-2026.pdf`. The previous commit preserves the deleted design experiments and generated build files if they are ever needed again.
