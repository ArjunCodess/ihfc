# Current code review

The sketch was reviewed for timed-move accounting, marker preflight readings, start-button debounce, motor PWM failure handling and invalid inputs. Wall approaches now drive forward until the first valid ultrasonic reading at or below 200 mm; other forward legs use the same stop threshold. All five planned servo channels run through a PCA9685 driver, whose detection and I2C failures are checked before the mission can run. The current mission leaves every delivery servo open and drops both beams without moving afterward.

See `VERIFICATION.md` for the actual compiler and regression-test evidence, and `CALIBRATION.md` for the physical measurements still required.
