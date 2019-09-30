<h1 align="center">Milestone 1: Figure 8</h1>

# Objectives

   The objective of this milestone is to make the robot successfully traverse a line and complete a figure eight pattern.
   
   **Materials:**
   - 1 Arduino Uno
   - 1 USB A/B cable
   - 2 Parallax Servos
   - 3 line sensors
   - 1 solderless breadboard
   - Team-made chassis
   
# Process #

1. We decided on our design to have three line sensors. The three line sensors would be in the front of the robot, spaced equally apart from each other. This way the robot could detect how far off the line it is, and we can code the robot to take the most suitable action to continue on the line.

2. Before we started the milestone, we had two S-shaped pillars that would hold the left and right line sensors in place and the third one was attached to the pillar that housed the ball bearing. However, this was a problem because the heights of the sensors were not uniform; we discovered that the different heights would affect the output values of whether the sensors were detecting dark colors and light colors. Also, in our code, we were planning on calculating the threshold of what constitutes as "white" by measuring multiple points of the center sensor when it is placed on the line. If the center sensor is not aligned height-wise with the other two sensors, then the threshold will not match uniformly across all sensors. 
<p align="center">
   <img src="IMG_5232.png" height="60%" width="60%">
   <br>
   <a>Previous Design</a>
</p>

<p align="center">
   <img src="IMG_6779.png" height="60%" width="60%">
   <br>
   <a>Current Design</a>
</p>

   We changed the design to include a third S-pillar that would hold the center line sensor and keep all three sensors aligned.
   
3. Our next step was making sure the sensors were properly working. In our code, we first started off with debug functions.
```cpp
void isOn(int light)  //light = Center, Left, Right sensor analog values
{
   if (light < threshold) //threshold calculated from sampling cneter sensor
      Serial.println("On line!");
   else
      Serial.println("Not on line.");
}
```
   This function would detect whether a sensor is on the line or not. We debugged each of the sensors and viewed the output of this function in the serial monitor. This function was very important because it meant that our function that calculates the threshold was correct and that the threshold value worked for all three sensors.
   
4. Next, we tested the Parallax Servos. We modified the isOn(int light) function to be a boolean function and tested the Servos in our loop body.
```cpp
void goFor() //go forward
{
   servoR.write(86);
   servoL.write(95);
}
void goStop() //stop
{
   servoR.write(90);
   servoL.write(90);
}
void loop()
{
   if (isOn(C))
      goFor();
   else (!isOn(C))
      goStop();
}
```
   This debug function further reinforced that our sensors were working, and that the sensors and the Servos were interacting correctly.
   
5. Next, we brainstormed a way to ensure that the robot could stay on the line. By using the isOn() function on each of the sensors, we can detect where the robot is relative to the grid line and slightly turn left or right to get back on the line again. Because the line sensors are spaced out to the width of the line, we used if else statements for each scenario. For example, if only the center sensor is activated, then the robot is on the line and it continues going forward. If the left sensor is on but the right sensor is off, then the robot is slightly to the right of the line and it will slightly turn to the left. And so forth.

6. We then coded the functions that would make the robot turn slightly left or slightly right. These functions were similar to the goFor() function, but the values were slightly tweaked to make one wheel turn faster. With the logic and Servo functions in place, the robot was able to traverse the line with relative ease and multiple tests ensured the durability of our code.

<iframe width="560" align="center" height="315" src="https://www.youtube.com/embed/dLYaa0ZTXIk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

7. Next, we tackled on the challenge of turning on an intersection. We knew that the robot would have to turn if all three sensors were activated (indicating an intersection). We made two more functions called hardRight() and hardLeft(). After many trials, we discovered that to make the best turn at an intersection, one wheel would have to rotate forward fast, and the other would slightly rotate backwards. To test this function and its durability, we let the robot continuously rotate in a circle, which was a combination of our code for traversing a line and the code for rotating at an intersection.

<iframe width="560" align="center" height="315" src="https://www.youtube.com/embed/ygiE4MoYcr0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

8. Finally, we tackled on making the robot make a figure 8. We used cases and a counter to determine which way to turn.
```cpp
 switch(k) {
     case 1: ...
```

<iframe align="center" width="560" height="315" src="https://www.youtube.com/embed/gOGfBiWHEYg" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

