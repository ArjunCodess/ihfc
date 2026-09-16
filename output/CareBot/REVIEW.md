# Review

`CareBotESP32.ino` starts one route automatically after power-up initialization. GPIO32 is unused. Its first movement is forward toward the ultrasonic target, and the motors stop at the first valid reading of 200 mm or less.

Both IR sensors are mounted at the front and must report black together. The source logs every ultrasonic sample, changes in both IR states, motor commands and stops, and every servo target at 115200 baud.

The three bin servos rest at 90 degrees. Channels 0 and 2 move down to 20 degrees for the two MG995 2-bin mechanisms. Channel 1 moves up to 160 degrees for the 6-bin mechanism. Channels 3 and 4 release the two beams after separate left turns.

The motor controller still requires a current check because each channel drives a pair of motors. Real wheel speed, the 580 ms turn time, sensor height, and servo angles require calibration on the loaded robot.