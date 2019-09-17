# Lab 1
## Monday 09/09/2019

In this lab we got familiar with the arduino. Below is a list of the things that we demo'ed with the arduino board along with addition periferals such as a potentiometer and servo motor.

***1.Blinking an LED***

* We modified the blink sketch provided by the Arduino program to work for an external LED rather than the LED on the Arduino board. We tested this modified code on all the digital pins.

* We connected the LED to pin 5 and modified the blink code to alternate the state of the LED digital pin.
```cpp
void loop() {
  digitalWrite(newLED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(newLED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```

***2.Polling and displaying analog data***

* Next, we used a potentiometer to input different voltages and printed these values on the screen. We wrote a loop function that would output the circuit's analog value every half-second.

* We first started by intializing the pin that would be connected to the potentiometer and a global variable that would hold the potentiometer value.
```cpp
int pin = A3;
int val = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(OUT, OUTPUT);
  Serial.begin(9600);
}
```
* We then can print this out to the serial monitor
```cpp
void loop() {
  // put your main code here, to run repeatedly:
  val = analogRead(pin);
  Serial.println(val);
  delay(500);
}
```
  <iframe width="640" height="480" src="https://www.youtube.com/embed/ZsglpwnNFWA" frameborder="0" allowfullscreen></iframe>

***3.Analog(PWM) output***

* Once we understood how to use analog, we used PWM to create a 'analog output'. We used an LED attached to a digial pin to set it up as an output pin, and used the analog value from the potentiometer to change the brightness of the LED.

* We added in an digital output to pin 5.
```cpp
int OUT = 5;
```
* We then mapped the potentiometer value to the LED, and we scaled down the values to the appropriate duty cycle.
```cpp
int input=map(value, 0, 1014, 0, 255);
```
  
  <iframe width="640" height="480" src="https://www.youtube.com/embed/7_lrMVkOdfE" frameborder="0" allowfullscreen></iframe>
  
***4.Servo Motor Control***

* We then moved onto the Parallax Servos. We used the Arduino `Servo.h` library for all the functions and were able to control the motor speed using a potentiometer.
  
* We added in the Servo library and created a Servo object to represent our Parallax Servo.
```cpp
#include <Servo.h> 

Servo mtr;
```

* We then wrote the value of the potentiometer to the servo using the `Servo.write()` function.
```cpp
mtr.write(val);   // adjusts the servo speed and direction based on the potentiometer value
delay(20); 
```
  <iframe width="640" height="480" src="https://www.youtube.com/embed/YehDK1NYFsw" frameborder="0" allowfullscreen></iframe>

***5.Assembling and Running our Robot***
We made the skeleton of our robot with the provided chassis, battery, and ball bearings. We had some trouble trying to figure out a good set up for robot since it had two layers (one for breadboard, one for the Arduino). We were able to make the robot loop through a sequence of stopping, turning, and driving. 
  
* We first created two Servo objects, one for the left servo and one for the right servo
```cpp
Servo mtrL;
Servo mtrR;
```
* We then attached the Servo to pins 9 and 10 in the `setup()` function.
```cpp
void setup() {
  mtrL.attach(9);
  mtrR.attach(10);
}
```
* We then wrote the sequence of running, stopping, and turning. We did this by varying the argument in the `Servo.write()` function; 90 means no motion the values range to a min of 0 and max of 180.
```cpp
void loop() {
  mtrL.write(0);
  mtrR.write(180);

  delay(5000);

  mtrL.write(0);
  mtrR.write(0);

  delay(1000);

  mtrL.write(90);
  mtrR.write(90);

  delay(5000);
}
```
  <iframe width="640" height="480" src="https://www.youtube.com/embed/uxIBmEzqmEU" frameborder="0" allowfullscreen></iframe>
  
**EXTRA**
  - We practiced some soldering on our sensors that we plan on using for our next lab. The sensors will be used to detect the color of the tape and the color of the mat, and we can use the output from the sensor for some valuable information
  <iframe width="640" height="480" src="https://www.youtube.com/embed/ybc-_0FbX9I" frameborder="0" allowfullscreen></iframe>

