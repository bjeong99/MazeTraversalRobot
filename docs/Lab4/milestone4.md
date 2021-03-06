# Milestone 4

Objective: Have the base station display update in a basic way , communicate information with the robot, and implement robot detection.

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


## Basestation integration

### 1. Base Station Arduino Code

The other half of the base station, the Arduino, handles radio communication with the robots as well as the majority of the data processing. The arduino first is connected to the FPGA using a parallel communication bus which uses nine GPIO pins. Below is the code used in the main lopp of the arduino to interface with the radio module.

```c
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      int data_received;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.        
        done = radio.read( &data_received, 2 * sizeof( byte ) );
        Data_Interpreter(data_received);
      }
      
        // First, stop listening so we can tal
        // Now, resume listening so we catch the next packets.
        radio.startListening();
      }
```
We constantly poll the radio, checking to see if we are receiving a transmission from the robot has come. If it has, we wait for the transmission to complete then we pass the received data into our data decoder so we know what data to send to the FPGA. The data decoder first breaks down the two byte number into four, four bit numbers that are then decoded by four case statements which return -1 if the values are invalid. If either of the values return invalid, the transmit process exits and the arduino goes back to polling the radio. The X and Y coordinates, bits [11:4] are passed directly into the FPGA packet so that the correct block position can be displayed. Bits [3:0], which contain the wall positions relative to the robot is first translated and stored into an array of bools. Finally, the walls are rotated by a rotation matrix implemented as a case statement so that the wall positions are in the reference fram of the robot. The code for this is shown below.

```c
 /* Matrix rotation operation to put robot reference frame in base station reference fram */
    switch(ori){
          case 0: buff[0] = wall[0]; buff[1] = wall[1]; buff[2] = wall[2]; buff[3] = wall[3]; break;
          case 1: buff[0] = wall[3]; buff[1] = wall[0]; buff[2] = wall[1]; buff[3] = wall[2]; break;
          case 2: buff[0] = wall[2]; buff[1] = wall[3]; buff[2] = wall[0]; buff[3] = wall[1]; break;
          case 3: buff[0] = wall[1]; buff[1] = wall[2]; buff[2] = wall[3]; buff[3] = wall[0]; break;
    }
```

After the rotation, the arduino is ready to pass the wall values onto the FPGA so they can be displayed. This is handled by looping over the buff[] array, and if there is a wall there, a packet is formed and sent to the FPGA so that wall can be drawn. Each wall is drawn one by one based off the way that the FPGA has been implemented. The code for the transmitting is shown below.

```c
  int j, sendByte;
  for (j = 0; j < 4; j++){
      if (buff[j] == 1){
        sendByte = (j << 6) + ((xSend) << 3) + (8 - ySend); // Format packet for FPGA
        //Serial.print(sendByte); //For debugging
        parallelTransmit(sendByte);
      }
  }
```

Within the parallel write function, we write to each of the GPIO pins on the arduino by shifting the bits of the input byte. Once this is done we clock the FPGA high for 10ms, then low then high again for another 10ms. The second write funciton was added after we noticed that the FPGA would occasionally miss the clock due to poor input signal integrity of the resistor divider used to bring the 3.3V of the FPGA and the 5V of the arduino. The parallel write funciton is shown below.

```c
    sendByte = (0x00FF & sendByte); // Assert 16 bit number
    digitalWrite( CLK, LOW);        // Assert Clock Low

    /* Write the GPIO using psuedo shift register, Keeping CLK Low */
      digitalWrite( BIT0, (sendByte >> 0 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT1, (sendByte >> 1 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT2, (sendByte >> 2 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT3, (sendByte >> 3 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT4, (sendByte >> 4 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT5, (sendByte >> 5 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT6, (sendByte >> 6 & 0x0001) ? HIGH:LOW);
      digitalWrite( BIT7, (sendByte >> 7 & 0x0001) ? HIGH:LOW);

    /* CLock The FPGA */
      digitalWrite(CLK, HIGH);
      delay(10);
      digitalWrite(CLK, LOW); 
```

### Demonstration
<iframe width="560" height="315" src="https://www.youtube.com/embed/ZRnJ6TjiB80" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="560" height="315" src="https://www.youtube.com/embed/FoZ5C8XUYHY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
