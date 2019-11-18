<h1 align="center">Milestone 3: Maze Algorithm and Robot Detection </h1>

# Objectives

   The objective of this milestone is to implement an algorithm for efficient maze exploration and enable the robot to avoid other robots through IR LEDs and IR sensors.
  
  **Additional Materials:**
  - existing robot design
  - IR LEDs
  - IR sensor
  
# Process
1. We decided to use DFS instead of BFS. From lecture and researching more online, BFS has the risk of taking up too much memory on the Arduino; the Arduino memory capacity is quite small, so we opted for DFS. 

2. To implement DFS, we decided that we needed a stack to push our visited nodes, a boolean array that represents the whole grid and represents which nodes had already been visited, and a node type that contains x and y coordinates. We also realized that keeping track of cardinal directions was important; DFS depends on orientation and the priority of cardinal directions, so keeping track of the robot's local cardinal direction would make it easier to know which way to turn. Finally, a boolean value called backtrack was necessary to keep track of whether the robot is in DFS mode or backtrack mode.
```c
int current_dir;
// dir
// north: 0
// east: 1
// south: 2
// west: 3

typedef struct
{
  int x;
  int y;
} node;

const int columns = 4;
const int rows = 5;
bool visited_nodes[columns][rows];

node current_node;

StackArray<node> stack;

bool back_track;
```

3. To implement DFS, we had to keep track of the current direction and make choices based on direction priority. In the loop, when the robot arrives at an intersection and if `backtrack == false`, then it would call `dfs()`. In `dfs()`, it would set the respective current node in the boolean array as true and check the robot's options. The robot would only proceed to a direction if there is no wall in that direction and if the next node in that direction has not been visited. It would check these options for all direction in the NESW order. 
```c
visited_nodes[current_node.x][current_node.y] = true;
  // current_dir = NORTH
  if (current_dir == 0)
  {
    // north, go north
    if (!isWall(distC) && visited_nodes[current_node.x][current_node.y + 1] == false)
    {
      Serial.println("DFS, North head North");
      Serial.println();
      current_dir = 0;
      stack.push(current_node);
      current_node.y = current_node.y + 1;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(250);
    }
```
The following bit of code would set the new cardinal direction and push the current node onto the stack (for backtracking) before changing the current coordinate to the prospective coordinate. The code would then tell the robot what direction to move.

   However, if none of the cases matched, the code would set `backtrack = true`.
```c
// north, u turn to south, backtrack
    else
    {
      goStop();
      Serial.print(2);
      Serial.print(", ");
      Serial.println(current_dir);
      back_track = true;
    }
```

4. In the loop body, if `backtrack == true`, it would then call `backtrack()`. In this function, it peeks at the top node on the stack and receives its coordinates. Because the coordinate that is peeked must always be one step away from the current node (nodes on stack are all one step away from each other and the top node is always one step away from the current node by design), we check the robot's current direction and the direction the robot needs to take to go from the current node to the peeked node. Once the direction is determined, the node is popped off the stack and becomes the current node, a new direction is set, and backtrack is set to false again.
```c
node next_node = stack.peek();
  //north
  if (current_dir == 0)
  {
    // north and if next node is north
    if (current_node.x == next_node.x && current_node.y == next_node.y - 1)
    {
      Serial.println ("BackTrack, North head North");
      Serial.println();
      current_dir = 0;
      current_node = stack.pop();
      back_track = false;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(200);
    }
```

5. Finally, the robot only reaches a stop when the stack is empty. The very first node that is added onto the stack is the origin node, and when the robot finished exploring the maze, it will backtrack, meaning it will pop the nodes off the stack and traverse all the nodes. Because of this design, when the stack is empty, the robot should stop at the origin node. 
```c
if (stack.isEmpty())
    {
      C = getC();
      L = getL();
      R = getR();
      goFor();
      delay(200);
      while (isOn(C) && isOn(R) && isOn(L))
      {
       goStop();
       Serial.println("Done");
      }
    }
```

<iframe width="560" height="315" src="https://www.youtube.com/embed/fJcBd08CyLw" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

<iframe width="560" height="315" src="https://www.youtube.com/embed/fC8wkf0bFOg" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

# Improvements
Some improvements we thought were necessary was changing the design of the robot. The robot does not have a Schmidt trigger and it very top heavy, so the sudden Servo motions make the robot shakea lot. This unwanted movement causes the sensors to incorrectly detect intersetions and grid lines. We also decided that soldering the circuit would be much better. During testing, the wires kept falling off the breadboard and it costed a lot of time to repair the robot many times.
