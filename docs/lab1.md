# Lab 1
## Monday 09/09/2019

In this lab we got familiar with the arduino. Below is a list of the things that we demo'ed with the arduino board along with addition periferals such as a potentiometer and servo motor.

1.Blinking an LED
  - We modified the blink sketch provided by the Arduino program to work for an external LED rather than the LED on the Arduino board. We tested this modified code on all the digital pins.

2.Polling and displaying analog data
  - Next, we used a potentiometer to input different voltages and printed these values on the screen. We wrote a loop function that would output the circuit's analog value every half-second.
  <iframe width="640" height="480" src="https://www.youtube.com/watch?v=ZsglpwnNFWA" frameborder="0" allowfullscreen></iframe>

3.Analog(PWM) output
  - Once we understood how to use analog, we used PWM to create a 'analog output'. We used an LED attached to a digial pin to set it up as an output pin, and used the analog value from the potentiometer to change the brightness of the LED.
  <iframe width="640" height="480" src="https://www.youtube.com/watch?v=7_lrMVkOdfE" frameborder="0" allowfullscreen></iframe>
  
4.Servo Motor Control
  - We then moved onto the Parallax Servos. We used the Arduino `Servo.h` library for all the functions and were able to control the motor speed using a potentiometer.
  <iframe width="640" height="480" src="https://www.youtube.com/watch?v=YehDK1NYFsw" frameborder="0" allowfullscreen></iframe>

5.Assembling and Running our Robot
  - We made the skeleton of our robot with the provided chassis, battery, and ball bearings. We had some trouble trying to figure out a good set up for robot since it had two layers (one for breadboard, one for the Arduino). We were able to make the robot loop through a sequence of stopping, turning, and driving. 
  <iframe width="640" height="480" src="https://www.youtube.com/watch?v=uxIBmEzqmEU" frameborder="0" allowfullscreen></iframe>
  
**EXTRA**
  - We practiced some soldering on our sensors that we plan on using for our next lab. The sensors will be used to detect the color of the tape and the color of the mat, and we can use the output from the sensor for some valuable information
  <iframe width="640" height="480" src="https://www.youtube.com/watch?v=ybc-_0FbX9I" frameborder="0" allowfullscreen></iframe>

