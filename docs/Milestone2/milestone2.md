<h1 align="center">Milestone 2: Circling Walls</h1>

# Objectives

   The objective of this milestone is to 1. circle an arbitrary set of walls through right hand following and 2. a demonstration of the robot performing line tracking and walls together.
   
   **Additional Materials:**
   - existing robot design
   - CD4051CBN multiplexer
   - 4 LEDs
   - 3 Sharp IR Sensors (GP2Y0A41SK0F)
   
<p align="center">
   <img src="IMG_6348.png" height="60%" width="60%">
   <br>
   <a>Current Design</a>
</p>

# Process
1. We decided to use 3 IR sensors for our wall detection (1 on the right, 1 on the left, and 1 on the front). This would result in 6 analog pins being used up. To reduce this down, we implemented the **CD4051CBN multiplexer**. With this component, we could connect the 3 line sensors and the 3 IR sensors using only one analog pin.

<p align="center">
   <img src="Screen Shot 2019-10-11 at 8.53.33 AM.png" height="60%" width="60%">
   <br>
   <a>Multiplexer Design</a>
</p>

2. With help from an online guide (https://cityos.io/tutorial/1958/Use-multiplexer-with-Arduino), we figured out how to wire the multiplexer and how to read values from each component
```c
// values for line sensors
  for (int i = 0; i < 3; i++) {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);
    
    sensorVal[i] = analogRead(A0);
  }

  // values for IR sensors
  for (int i = 3; i < totalChannels; i++) {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);

    float volt = analogRead(A0) * 0.0048828125;
    sensorVal[i] = 13 * pow(volt, -1);
  }
```
   The for loop would iterate through each component using the selector digital pins, and will read the analog value of each and store it in an array for later use. We also configured the analog value of the proximity sensors to convert it to a distance in centimeters for more easier use.
   
<p align="center">
   <img src="IMG_5915.png" height="60%" width="60%">
   <br>
   <a>Mux Circuit</a>
</p>

3. From milestone 1, we decided to change our turning methods. Before, we would turn right when it would detect a line and would try to turn while keeping a sensor in line with the white line. However, we saw that this method was not consistent and reliable because there were many variables that prevented a clean turn (i.e. robot not being perfectly straight, lines not being straight, etc). We decided to make it rotate instead of turn when it detects an intersection. 
```c
goFor();
        delay(750);
        hardLeft();
        delay(500);
        R = getR();
        while(!isOn(R)){
          R = getR();
          Serial.println(C);
        }
        goStop();
```

4. We then moved onto the logic for wall detection. To make it easier, we used boolean algebra to simplify what turns the robot would do in what conditions. This step shortened our code by a lot because we didn't create conidition statements for every possible combination of a wall being detected on the left, center, or right.

5. To visually see what our robot was "thinking", we also put LED's on the board. The green LED would indicate whenever it crossed an intersection, yellow would indicate wall detection and moving forward, the first red would indicate wall detection and moving left, and the second red would indicate wall detection and moving right. 

<p align="center">
   <img src="IMG_6969.png" height="60%" width="60%">
   <br>
   <a>LED Layout</a>
</p>

6. We then put it to the test! Some complications were:
   - jittering in the turns and line correction (solved by tweaking the servo numbers)
   - wheels falling off from too sharp turns (solved by screwing in wires)
   - some jittering due to weight distributed too much in the back (will solve by fixing robot design)
   Overall, the robot ran very well, and the wall detection code intertwined well with the line-tracking code.

<p align="center">
   <iframe width="560" height="315" src="https://www.youtube.com/embed/rQ8amvRA_LU" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
</p>
   
