<h1 align="center">Milestone 4 and Lab 4: Radio Communication and Full Robot Integration</h1>


# Objectives 

   The objective of this lab is to use the Nordic nRF24L01+ transceivers and the corresponding Arduino RF24 library to get the robot and the video controller to communicate with each other. Also, full robotic integration is expected of this lab.
   
   The objective of this milestone is for the robot to update the base station's display in a basic way, communicate information with the base station, and detect other robots and act upon that information.
  
  **Additional Materials:**
  - Nordic nRF24L01+ transceivers
  - Arduino RF24 library
  - existing robot
     - existing robot contains hardware to detect other robots from previous milestone
  
## Process: Full Robotic Integration

### 1. Motor Control
For motor contro, we used two Parallax servos attached to the robot. We would attach the Arduino pin to the Servo in `setup()` and set values to it with 90 being stop. For example, our forward function set the servos to `servoR.write(70)` and `servoL.write(110)`. The various functions to make the robot go forward and turn are apparent in our various videos. 

### 2. FFT: Tone Sensing

### 3. Robot Starts when it detects 950 Hz

### 4. Override Button
For testing in lab and in case the robot does not start with the 950 Hz tone, we attached an override button.
![alt text](https://components101.com/sites/default/files/component_pin/Push-button-Pinout.gif)

We connected the button to the 5V and attached to digital pin 3. It would pass 1 when pressed and 0 when it's not. To implement this in our code, we decided to create a global variable `can_start` which is initialized to `false`. 

```c
while (can_start == false)
  {
    goStop();
    can_start = digitalRead(push_button) || microphone_detect();;
  }
```

This while loop is written in the loop function, and it will be stuck in this while loop until `can_start` is true, which is when the button is pressed or 950 Hz is detected. When the while loop is excited, the code will never enter this while loop again and will continue on with the rest of the navigation code. 

### 5. Line Sensing and Following

We used three line sensors in front of the robot. They were spread far enough apart that the distance between two sensors is about the width of the line. To make it easier to find the threshold, we decided to average a sample in the setup function.

```c
while (i < 50)
{
  temp += analogRead(A1);
  i++;
}
threshold = temp / 50 + 350;
```

This way, we wouldn't need to update the threshold value for slightly different conditions.

To follow the line, we used the fact at a given time, both the right and left line sensors could not be on the line simultaneously. We deduced that if the left sensor is on the line but the right is not, the robot must veer right slightly, and vice versa.

```c
if (isOn(C) && !isOn(L) && !isOn(R))
    goFor();
  else if (isOn(L) && !isOn(R))
  {
    slightLeft();
  }
  else if (!isOn(L) && isOn(R))
  {
    slightRight();
  }
```
When it is at an intersection, all three line sensors must sense white, so we implemented turning functions that would only be called on intersections. Because using time delays were very inaccurate, we decided to call the turn function and continue to line following when the sensors detected another line.

```c
void turnLeft()
{
  goFor();
  delay(400);
  hardLeft();
  delay(700);
  R = getR();
  while (!isOn(R))
  {
    R = getR();
  }
  goStop();
}
```

### 6. Wall Sensing
To detect walls, we added short range IR sensors to the front and the sides of the robot. When testing the sensors with the Arduino, we were able to code a function that would convert the analog value to distance in centimeters. We then created a boolean function called `isWall(int distWall)` that would return a boolean value if the distance of a wall from the robot is less than the set threshold. 

```c
boolean isWall(int sensorDistance)
{
  if (sensorDistance < 20)
    return true;
  return false;
}
```

We used this boolean function to check where the walls are around the robot. We only check the location of walls at intersections since we can only turn to avoid walls on intersections.

In the navigation code, we use a matrix of boolean values the size of the maze to represent visited nodes and a stack array. Whenever the node is visited, that node is added to the stack and the boolean value of that coordinate in visited is set to true . The nodes in the stack then get popped when the robot enters backtrack mode and travels to the popped nodes to go back to the origin. To deal with unexplorable nodes, we would check whether the stack is empty rather than checking if all the elements in the visited array are set to true. The stack would only be empty when the robot has finished backtracking and is back at the origin.

```c
//stack is empty when it reaches (0, 1) so it must still go forward to (0, 0) to finish
while (stack.isEmpty())
{
      
  C = getC();
  L = getL();
  R = getR();
  if (isOn(C) && !isOn(L) && !isOn(R))
    goFor();
  else if (isOn(L) && !isOn(R))
  {
    slightLeft();
  }
  else if (!isOn(L) && isOn(R))
  {
    slightRight();
  }
  if (isOn(C) && isOn(R) && isOn(L))
  {
    radio_transmitter();
    while (1)
    {
      goStop();
      digitalWrite(finishPin, HIGH);
    }
  }
}
```

### 7. Robot Sensing and Avoidance

[Go here for Robot Sensing and Avoidance from Milestone 4 Portion](#process-acting-upon-robot-detection)

### 8. Navigation Algorithm, Finish LED
We decided that DFS was the best way to explore the maze efficiently. To implement DFS, we first created a boolean array `visited_nodes`, a stack array 'stack', and a boolean value 'backtrack'. Our prioritization was North, East, South, and West. The robot would swap between line following functions and DFS; line following is only used when traversing a line and DFS is called at intersections.

When the Robot enters DFS, it first marks the `current_node` as visited in the array. It then checks the `current_dir` and decides an action from there. Regardless of the `current_dir`, the robot will first try to continue heading in the forward direction. If there is a wall or robot in its path, it will then turn to the next available node, in priority order of North, East, South, and West. If the robot has to turn backward, it will exit DFS and perform no action except changing `back_track` to true. Then, the code will exit back to loop but will immediately jump to the code when all line sensors are active (the process through DFS is very fast and has virtually no delay so the robot is still on the intersection). However, now that `back_track` is true, it will jump to the backtrack() function.

In backtrack, it will peek at the node on top of the stack and declare it as `next_node`. Depending on the robot's direction, it will compare the relative direction of the next node to the current node. All nodes in the stack are one tile away from each other, and the robot will decide which way to turn to reach that next tile. We then turn `backtrack = false` when it makes that decision. We do this because there is a possibility that in backtracking, the robot can continue DFS to unvisited nodes, but if the surrounding nodes are already visited, DFS will turn `backtrack = true` again and backtrack will continue. 

### 9. FPGA/Base Station

### 10. Wireless Communication

## Process: Base Station


## Process: Acting Upon Robot Detection

### 1. Robot Detection Circuit
For robot detection previously, we found that using a raw phototransistor circuit with the Arduino did not produce a sensitive enough IR detector. We instead opted for a BJT circuit that would amplify the values that would be received from the phototransistor.

![alt text](https://www.electronicwings.com/public/images/user_images/images/Arduino/IR%20Communication/IR_Communication_1.png)

To make it easy to detect IR emitters, we created a bool function that can be called anytime in the loop or the DFS code to check the state of the phototransistor.

```c
bool detect(float ptValue)
{
  if (ptValue > 400)
  {
    digitalWrite(finishPin,HIGH);
    Serial.println("Detected");
    return true;
  }
  return false;
}
```

### 2. Brainstorm for Algorithms
   We realized that with our DFS code, we woul dhave to create two different solutions to robot detection since the actions between DFS and backtrack are very different.

### 3. Robot Detection During DFS
For DFS, the most efficient way was to think of a robot as a wall. When it senses another robot, it will proceed DFS but in another prioritized direction. For instance, if the robot is facing north and detects a robot in front of it, it will head east (east has higher priority over west) and continue DFS. Eventually, when the robot enters backtrack, it will continue DFS to the node that it had to miss.

To implement, this we simply changed the conditional statements when the robot wants to go forward. 
```c
 // current_dir = NORTH
  if (current_dir == 0)
  {
    // north, go north
    if (!isWall(distC) && visited_nodes[current_node.x][current_node.y + 1] == false && !detect(ptValue))
    {
      Serial.println(F("DFS, North head North"));
      Serial.println();
      current_dir = 0;
      stack.push(current_node);
      current_node.y = current_node.y + 1;
      goFor();
      delay(250);
    }
```

### 4. Robot Detection During Backtrack
For backtracking, the best way was to circle back one node and then circle back again and continue backtrack. Our backtrack algorithm had to make sure that each of the nodes on the stack were one tile away from one another, so we thought this was the fastest and best way to deal with robot detection.

```c
 if (detect(ptValue))
  {
    stack.push(current_node);
    stack.push(previous_node);
  }
  node next_node = stack.peek();
  previous_node = current_node;
```
Here, when another robot is detected, it will push the current node and the previous node it just backtracked from. This way, it will u-turn to avoid the robot, reach that previous node, and u-turn again to go to the pushed current node and continue backtrack. 

### 5. Video Demonstration
Here is a video that demonstrates the robot's actions when it detects a robot during its DFS and during its backtrack mode.

<iframe width="560" height="315" src="https://www.youtube.com/embed/OS-aU18pEH4" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

The green LED lights up when it detects another robot (in our case, an IR emitter circuit) and the robot will continue DFS. The robot will only turn when it detects another robot at less than a foot away. We thought it was unnecssary to detect another robot that is further than an intersection away because the robot would take an unnecessary turn that would add time cost and we expect that when our robot continues and the opposing robot reaches the intersection, that robot would detect our robot and avoid it. We were able to have our circuit detect IR emitters more than a foot away as shown in our previous milestone. 
