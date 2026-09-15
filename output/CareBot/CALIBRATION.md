# Calibration record

The source defaults below are examples, not measurements of the assembled robot. Record final values and change the corresponding constants near the top of `CareBotESP32.ino` before a physical demonstration.

| Setting | Packaged value | Your measured value |
| --- | --- | --- |
| Motor rated voltage | 12 V | __________ |
| Motor rated speed | 500 RPM | __________ |
| Motor stall current at rated voltage | Unknown | __________ |
| Battery voltage / capacity | Unselected | __________ |
| Servo models / regulated supply rating | Unselected | __________ |
| Wheel diameter | Unmeasured | __________ |
| `INVERT_LEFT` / `INVERT_RIGHT` | false / true | __________ |
| `DRIVE_PWM` / `SLOW_PWM` / `TURN_PWM` | 145 / 100 / 125 | __________ |
| `LEFT_TRIM` / `RIGHT_TRIM` | 0 / 0 | __________ |
| `DRIVE_MM_PER_SECOND` | 180 | __________ |
| `REVERSE_MM_PER_SECOND` | 160 | __________ |
| `TURN_LEFT_MS` / `TURN_RIGHT_MS` | 580 / 580 | __________ |
| `WALL_STOP_MM` (one threshold for all forward travel) | 200 | User-specified 20 cm |
| `BLACK_LEVEL` | LOW | __________ |
| `MIDDLE_MARKER_NUMBER` | 1 | __________ |
| `FIRST_OUTLET_CORRECTION_MM` | 0 | __________ |
| `MIDDLE_OUTLET_CORRECTION_MM` | 0 | __________ |
| `LAST_OUTLET_CORRECTION_MM` | 0 | __________ |
| `BEAM_LANE_SHIFT_MM` | 0 | __________ |
| `CLOSED_DEG`, bins A/middle/B, right/front grippers | 15, 15, 15, 35, 35 | __________ |
| `OPEN_DEG`, same order | 100, 100, 100, 110, 110 | __________ |
| `SERVO_MIN_US` / `SERVO_MAX_US` | 1000 / 2000 | __________ |
| `RELEASE_MS` | 1100 | __________ |

Positive outlet offsets move forward in the robot's current heading; negative values reverse. At the final rightward leg, a positive beam lane shift moves up in the field image, and a negative value moves down. The final beam release is at one pose, so both mounts must align their loads from that same pose.

Measure travel speed at the configured PWM with the robot loaded. A nominal 500 RPM motor speed does not determine millimetres per second without wheel diameter and actual loaded speed. Tune left turns with both beams loaded and enough room for their swept area. The 200 mm reading is from the ultrasonic face to the reflecting surface; account for robot overhang and coasting when checking that it actually stops before contact.

## Physical run record

| Attempt/date | Code/settings version | 2 kits | 6 kits | 2 kits | Both beams dropped clear | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| __________ | __________ | ____ | ____ | ____ | ____ | __________ |
| __________ | __________ | ____ | ____ | ____ | ____ | __________ |
| __________ | __________ | ____ | ____ | ____ | ____ | __________ |

No physical results are pre-filled because no physical robot tests were performed in this session.
