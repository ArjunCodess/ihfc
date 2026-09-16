# Demonstration guide

## Before the showcase

- Install Arduino IDE, the Espressif ESP32 board package and the board's USB driver if needed. Open and compile the packaged sketch before going offline.
- Complete the measured entries in `CALIBRATION.md` and enter them in the sketch. Upload that calibrated source and keep a copy of it with your records.
- Check the motors' voltage/current requirements, power wiring, wheel directions and loaded turning clearance.
- Test each flap with its actual load and each gripper with its beam. Both beams must fall clear while the robot remains still.
- Check that the front hardware does not obstruct ultrasonic readings and that both front IR sensors distinguish black tape from the floor.
- Run the full route on the physical field. Record successful and failed attempts; the supplied test logs are software tests, not evidence of a physical run.

## Live run

1. Open `SHOWCASE.html` on the display. Keep Arduino IDE ready with the actual sketch if someone asks about the code.
2. With the mechanisms clear and the wheels raised, power on and confirm the servo rest positions.
3. Load 2 kits into bin A, 6 into the middle bin, and 2 into bin B. Load the first and second beams into their holders.
4. Place the robot at the top right, facing left, with the calibrated starting alignment.
5. Put the robot at the starting position and connect the battery. The route starts automatically.
6. Let the robot complete the 2–6–2 route and release each beam after its left turn. It finishes stationary with the release servos open.
7. For another run, disconnect power, clear the mechanisms, reload, and reconnect power at the starting position.

For an unexpected movement, use the main power switch. If Serial Monitor is connected, `x` stops the motors in software. A stop leaves the servos at their current positions.

## One-minute explanation

“This robot delivers preloaded medical kits on a fixed field. We use an ESP32 to control four geared drive motors and five servos. A front ultrasonic sensor measures the distance to the walls, and two downward-facing IR sensors at the front confirm black arena tape.

“We start at the top right and travel to the top left. The robot turns down and opens the first bin to drop two kits. At the middle delivery area it drops six, and at the bottom left it drops the last two. It then follows the remaining wall stops and releases each beam after a separate left turn.

“The mechanisms are simple: opening a flap or gripper lets the load drop. Every servo stays open after delivery, and the robot stays stopped after dropping the beams. The software has compiled for the ESP32 and passed simulated fault and route tests. Physical accuracy depends on the calibration we perform on the real robot.”

If you have completed physical testing, replace the last sentence with your recorded result, including the number of attempts. Do not claim physical success based on the supplied software logs.

## Questions you may get

**How does it know where it is?** It does not maintain an absolute position. It follows the known route using wall stops, marker detection and calibrated timed movements.

**Does it follow the black line continuously?** No. Two front binary IR sensors confirm a crossing marker together; they do not steer continuously along the line.

**How does it count six kits?** It does not count them electronically. The middle bin is loaded with six, and one flap releases the group.

**Why five servos?** Three open the bins and two open the beam grippers.

**What happens if a sensor fails?** Invalid echoes or a missing marker stop the program. Reset is needed before another attempt.

**Why L298N?** It provides separate left-side and right-side direction and PWM inputs. The module must safely carry the combined startup and stall current of the two motors on each channel.
