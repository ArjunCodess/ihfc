# Verification

`CareBotESP32/CareBotESP32.ino` is compiled for `esp32:esp32:esp32` with warnings enabled. The route starts automatically after initialization. GPIO32 is unused.

The source stops the motor outputs when the ultrasonic reading reaches 200 mm or less, an echo disappears, a movement times out, PCA9685 communication fails, or Serial `x` is received. It logs motor commands, ultrasonic readings, front IR state changes, and servo targets at 115200 baud.

The firmware commands the three bin servos from a 90-degree rest position. Channels 0 and 2 move in the negative direction to 20 degrees. Channel 1 moves in the positive direction to 160 degrees. Channels 3 and 4 control the beam releases.

The official photographs show four motors. The L298N must be checked against the combined startup and stall current of the two motors connected to each channel. Physical calibration is still required for motor speed, turn time, sensor height, servo travel, and release geometry.