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



