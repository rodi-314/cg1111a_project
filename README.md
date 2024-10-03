# CG1111A — Engineering Principles and Practice I Project

This group project was completed as part of the CG1111A Engineering Principles and Practice I course taught at the National University of Singapore (NUS). For more informtion and details, view the full report [here](<The A-maze-ing Race Project Report.pdf>).

![image](https://github.com/user-attachments/assets/b9176ba9-46c4-4a4c-bda4-31c81ab72ad3)
![image](https://github.com/user-attachments/assets/fdbdb240-a66d-4d9c-91a7-9f0a38675bba)

## 1 | Description of Overall Algorithm

![image](https://github.com/user-attachments/assets/e2593642-927b-4000-923e-a7b7737db367)

_Figure 1. Flowchart of overall maze-solving algorithm_

1.	The mBot first checks whether it has celebrated (i.e., reached the end of the maze). If it has celebrated, stop indefinitely by returning prematurely from loop(), otherwise, continue with loop().
2.	The mBot moves forward in a straight line until it detects a black strip underneath it, using a line follower. It constantly adjusts its movement by nudging left or right to move in a straight line and avoid walls. It decides whether to nudge left or right based on the distance measured using the ultrasonic sensor, and the reading obtained from the IR sensor (see _2.2 | Moving in a Straight Line_ for more details).
3.	If it detects a black strip, it stops and reads the colour of the paper underneath it, using a colour sensor. 
4.	The built-in LED display at the top of the mBot lights up to indicate which colour has been detected (e.g., if the colour red is detected, red is shown on the LED display).
5.	The mBot then moves according to the colour detected (see _Figure 2_). These movements are hard-coded, and the mBot does not measure the distances and readings from the ultrasonic sensor and the IR sensor respectively during these hard-coded movements, meaning no nudging occurs.
6.	Steps 1 to 5 are then repeated indefinitely.

![image](https://github.com/user-attachments/assets/59ca8d58-b434-446b-a4b7-0926f5c8326b)

_Figure 2. Colour interpretation for the colour sensing challenge_

## 2 | Implementation Details and Calibration of the Various Subsystems 

## 2.1. Motion

To code the turning motions of the mBot, values provided in _mBot introduction.pdf_ were used initially. However, it was discovered through preliminary testing that these values were not suitable for all the mazes, due to variation in the maze surfaces (see _3.2 | Variation in Maze Surfaces_). Hence, different values for turning delays and motor speeds were experimented with, and the final values used are: a delay value of 340 milliseconds per 90° turn, and a motor speed of 220. 

The following motion-related functions were defined in the code:

| Function | Corresponding Colour	| Description |
| -------- | -------------------- | ----------- |
| stopMotors() |	-	| Causes the mBot to halt immediately. Both motors are stopped by this function. |
| goStraight()	| -	| Causes the mBot to move in a perfectly straight line when no nudging is required, until the mBot detects a black strip. Both motors move in the forward direction at the same speed to achieve this. |
| goForward()	| -	| Causes the mBot to move one grid forward. This function is only invoked in other functions, such as doubleLeft().  This is achieved by calling goStraight() and delaying for 800 milliseconds. |
| turnLeft() | Red	| Causes the mBot to make a 90° left turn. The left motor moves backwards while the right motor moves forwards, with both motors running at the same speed, to achieve this stationary turn. The turning delay used is 340 milliseconds. |
| turnRight() |	Green	| Causes the mBot to make a 90° right turn. The left motor moves forwards while the right motor moves backwards, with both motors running at the same speed, to achieve this stationary turn. The turning delay used is 340 milliseconds. |
| doubleLeft()	| Purple	| Causes the mBot to turnLeft(), goForward(), then turnLeft() again. |
| doubleRight()	| Blue	| Causes the mBot to turnRight(), goForward(), then turnRight() again. |
| spin() |	Orange	| Causes the mBot to make a 180° turn on the spot. The left motor moves backwards while the right motor moves forwards, with both motors running at the same speed, to achieve this stationary turn. The turning delay used is 680 milliseconds (340 milliseconds * 2). |
| nudgeLeft() |	-	| Causes the mBot to veer leftwards to avoid colliding with the right wall. The right motor moves faster than the left motor by 69 units to nudge left. This function is used in the algorithm that ensures that the mBot moves in a straight line, by utilising the ultrasonic sensor and the IR sensor (to be discussed in _2.2 &#124; Moving in a Straight Line_). |
| nudgeRight()	| -	| Causes the mBot to veer rightwards to avoid colliding with the left wall. The left motor moves faster than the right motor by 69 units to nudge right. This function is used in the algorithm that ensures that the mBot moves in a straight line, by utilising the ultrasonic sensor and the IR sensor (to be discussed in _2.2 &#124; Moving in a Straight Line_). |


_Figure 3. Table illustrating each motion-related function, their related colour, and purpose_

