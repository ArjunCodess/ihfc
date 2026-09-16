# Robot model consistency check

The photographs in `images/` are the official record of the built robot. The Blender screenshots are useful for understanding the layout, but the real robot takes priority when they differ.

## What matches

- The robot has a low rectangular chassis and four wheels.
- It has three kit sections for the 2, 6 and 2 kit groups.
- A front ultrasonic sensor is visible on the built robot.
- The ESP32 program uses separate left-side and right-side drive commands, which fits a four-wheel skid-steer chassis when the two motors on each side are controlled together.
- Five servo channels remain correct: three kit releases and two beam releases.

## Inconsistencies found

| Repository statement | Official model | Resolution |
| --- | --- | --- |
| Two wheels and one caster | Four driven wheels | Component list and submission document now use four geared motors and four wheels. |
| One front beam and one right beam | The model does not support those exact names clearly | Servo channels 3 and 4 are now called first and second beam releases. |
| Two motor loads on one L298N | Four geared motors are visible | The program controls left and right side pairs. The L298N must be checked against the combined startup and stall current of two motors per channel. |
| Drawn top-view robot in the submission PDF | User supplied official photographs | The drawing was removed and the three photographs in `images/` were added. |
| Fixed 460 by 260 mm size and mass below 3 kg | No scale or measured mass was supplied | These estimates were removed. The built robot must be measured before submission. |

## Details not confirmed by the images

- The two downward-facing IR sensors are not clearly visible.
- The main power switch is not clearly identifiable.
- The complete linkage for both beam releases is not clear.
- The exact motor, battery, regulator and servo current ratings are not readable.
- The finished length, width, height and mass are not known.

These items need a physical check. They should not be guessed from the Blender model.
