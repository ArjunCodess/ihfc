# Current code review

The sketch was reviewed and corrected for timed-move accounting, wall-target overshoot, marker preflight readings, start-button debounce, motor PWM failure handling and invalid inputs. All five servos now run through a PCA9685 driver, whose detection and I2C failures are checked before the mission can run. The current mission leaves every delivery servo open and drops both beams without moving afterward.

See `VERIFICATION.md` for the actual compiler and regression-test evidence, and `CALIBRATION.md` for the physical measurements still required.
