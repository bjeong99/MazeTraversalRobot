#include <StackArray.h>
#include <Servo.h>

//#define lightR A2
//#define lightL A1
//#define lightC A0

#define onButton 7

Servo servoR;
Servo servoL;

volatile int R;
volatile int L;
volatile int C;

volatile int distR;
volatile int distL;
volatile int distC;

int k = 1;

unsigned long threshold;
float thresholdPT;

int totalChannels = 6;

int addressX = 5;
int addressY = 6;
int addressZ = 7;

int X = 0;
int Y = 0;
int Z = 0;

int distanceThreshold = 20;

int intersectPin = 2;
int leftPin = 4;
int forPin = 3;
int rightPin = 8;

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
const int rows = 4;
bool visited_nodes[columns][rows];

node current_node;

StackArray<node> stack;

bool back_track;
void setup()
{
  // set back track to false
  back_track = false;
  // set all values of visited_nodes as false
  for (int i = 0; i < columns; i++)
  {
    for (int j = 0; j < rows; j++)
    {
      visited_nodes[i][j] = false;
    }
  }
  // set current_dir as north
  current_dir = 0;
  // set current_node as (0,1)
  current_node.x = 0;
  current_node.y = 0;
  stack.push(current_node);
  current_node.x = 0;
  current_node.y = 1;
  visited_nodes[0][0] = true;
  // LED pin
  pinMode(intersectPin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(forPin, OUTPUT);
  pinMode(rightPin, OUTPUT);
  // Prepare address pins for output
  pinMode(addressX, OUTPUT);
  pinMode(addressY, OUTPUT);
  pinMode(addressZ, OUTPUT);

  // Prepare read pin
  pinMode(A0, INPUT);

  // put your setup code here, to run once:
  servoR.attach(9);
  servoL.attach(10);

  Serial.begin(9600);

  X = bitRead(0, 0);
  Y = bitRead(0, 1);
  Z = bitRead(0, 2);

  digitalWrite(addressX, X);
  digitalWrite(addressY, Y);
  digitalWrite(addressZ, Z);

  //Define threshold of the Robot (Varies day to day)
  int i = 0;
  int temp = 0;
  float temp2 = 0;
  while (i < 50)
  {
    temp += analogRead(A0);
    i++;
  }
  threshold = temp / 50 + 500;
  Serial.print("Threshold: ");
  Serial.println(threshold);
}

// checks if light is on the line or not (works)
boolean isOn(int light)
{
  if (light < threshold)
    return true;
  else
    return false;
}

boolean isWall(int sensorDistance)
{
  if (sensorDistance < distanceThreshold)
    return true;
  return false;
}

//servo functions
void goFor()
{
  servoR.write(81);
  servoL.write(99);
}
void goBack()
{
  servoR.write(95);
  servoL.write(85);
}

void goStop()
{
  servoR.write(90);
  servoL.write(90);
}

void slightRight()
{
  servoR.write(88);
  servoL.write(97);
}

void hardRight()
{
  servoR.write(95);
  servoL.write(95);
}

void slightLeft()
{
  servoR.write(83);
  servoL.write(92);
}

void hardLeft()
{
  servoR.write(85);
  servoL.write(85);
}

// return sensor values if values needed in nested loop
int getC()
{
  X = bitRead(0, 0);
  Y = bitRead(0, 1);
  Z = bitRead(0, 2);

  digitalWrite(addressX, X);
  digitalWrite(addressY, Y);
  digitalWrite(addressZ, Z);

  return analogRead(A0);
}

int getR()
{
  X = bitRead(1, 0);
  Y = bitRead(1, 1);
  Z = bitRead(1, 2);

  digitalWrite(addressX, X);
  digitalWrite(addressY, Y);
  digitalWrite(addressZ, Z);

  return analogRead(A0);
}

int getL()
{
  X = bitRead(2, 0);
  Y = bitRead(2, 1);
  Z = bitRead(2, 2);

  digitalWrite(addressX, X);
  digitalWrite(addressY, Y);
  digitalWrite(addressZ, Z);

  return analogRead(A0);
}

void turnLeft()
{
  digitalWrite(leftPin, HIGH);
  goFor();
  delay(500);
  hardLeft();
  delay(500);
  R = getR();
  while (!isOn(R))
  {
    R = getR();
  }
  goStop();
}

void turnRight()
{
  digitalWrite(rightPin, HIGH);
  goFor();
  delay(500);
  hardRight();
  delay(500);
  L = getL();
  while (!isOn(L))
  {
    L = getL();
  }
  goStop();
}

void uTurn()
{
  digitalWrite(leftPin, HIGH);
  goFor();
  delay(250);
  servoR.write(84);
  servoL.write(75);
  delay(250);
  R = getR();
  while (!isOn(R))
  {
    R = getR();
  }
  servoR.write(84);
  servoL.write(75);
  delay(250);
  R = getR();
  while (!isOn(R))
  {
    R = getR();
  }
  goStop();
}

float ptCurrent()
{
  float result = analogRead(A4);
  float voltage = result * (5.0 / 1023.0);
  float current = 1000 * voltage / 178;
  return current;
}

bool onGrid(int x, int y)
{
  if (x >=0 && x < columns - 1 && y >= 0 && y < rows - 1)
    true;
  else
    false;
}
void dfs()
{
  Serial.println("Is on Intersect");
  digitalWrite(intersectPin, HIGH);
  goStop();
  delay(250);
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
    // north, turn east
    else if (!isWall(distR) && visited_nodes[current_node.x + 1][current_node.y] == false)
    {
      Serial.println("DFS, North head East");
      Serial.println();
      current_dir = 1;
      stack.push(current_node);
      current_node.x = current_node.x + 1;
      turnRight();
    }
    // north, turn west
    else if (!isWall(distL) && visited_nodes[current_node.x - 1][current_node.y] == false)
    {
      Serial.println("DFS, North head West");
      Serial.println();
      current_dir = 3;
      stack.push(current_node);
      current_node.x = current_node.x - 1;
      turnLeft();
    }
    // north, u turn to south, backtrack
    else
    {
      goStop();
      Serial.print(2);
      Serial.print(", ");
      Serial.println(current_dir);
      back_track = true;
    }
  }
  // current_dir = EAST
  else if (current_dir == 1)
  {
    // east, go forward to east
    if (!isWall(distC) && visited_nodes[current_node.x + 1][current_node.y] == false)
    {
      Serial.println("DFS, East head East");
      Serial.println();
      current_dir = 1;
      stack.push(current_node);
      current_node.x = current_node.x + 1;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(250);
    }
    // east, turn left to north
    else if (!isWall(distL) && visited_nodes[current_node.x][current_node.y + 1] == false)
    {
      Serial.println("DFS, East head North");
      Serial.println();
      current_dir = 0;
      stack.push(current_node);
      current_node.y = current_node.y + 1;
      turnLeft();
    }
    // east, go right to south
    else if (!isWall(distR) && visited_nodes[current_node.x][current_node.y - 1] == false)
    {
      Serial.println("DFS, East head South");
      Serial.println();
      current_dir = 2;
      stack.push(current_node);
      current_node.y = current_node.y - 1;
      turnRight();
    }
    // else back track
    else
    {
      goStop();
      Serial.print(1);
      Serial.print(", ");
      Serial.println(current_dir);
      back_track = true;
    }
  }
  // current_dir = SOUTH
  else if (current_dir == 2)
  {
    //south, go forward to go south
    if (!isWall(distC) && visited_nodes[current_node.x][current_node.y - 1] == false)
    {
      Serial.println("DFS, South head South");
      Serial.println();
      current_dir = 2;
      stack.push(current_node);
      current_node.y = current_node.y - 1;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(250);
    }
    //south, turn left to go east
    else if (!isWall(distL) && visited_nodes[current_node.x + 1][current_node.y] == false)
    {
      Serial.println("DFS, South head East");
      Serial.println();
      current_dir = 1;
      stack.push(current_node);
      current_node.x = current_node.x + 1;
      turnLeft();
    }
    //south, go right to go west
    else if (!isWall(distR) && visited_nodes[current_node.x - 1][current_node.y] == false)
    {
      Serial.println("DFS, South head West");
      Serial.println();
      current_dir = 3;
      stack.push(current_node);
      current_node.x = current_node.x - 1;
      turnRight();
    }
    //south, backtrack to north
    else
    {
      goStop();
      Serial.print(2);
      Serial.print(", ");
      Serial.println(current_dir);
      back_track = true;
    }
  }
  // current_dir = WEST
  else if (current_dir == 3)
  {
    //west, go for to go west
    if (!isWall(distC) && visited_nodes[current_node.x - 1][current_node.y] == false)
    {
      Serial.println("DFS, West head West");
      Serial.println();
      current_dir = 3;
      stack.push(current_node);
      current_node.x = current_node.x - 1;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(250);
    }
    //west, turn right to north
    else if (!isWall(distR) && visited_nodes[current_node.x][current_node.y + 1] == false)
    {
      Serial.println("DFS, West head North");
      Serial.println();
      current_dir = 0;
      stack.push(current_node);
      current_node.y = current_node.y + 1;
      turnRight();
    }
    //west, turn left to go south
    else if (!isWall(distL) && visited_nodes[current_node.x][current_node.y - 1] == false)
    {
      Serial.println("DFS, West head South");
      Serial.println();
      current_dir = 2;
      stack.push(current_node);
      current_node.y = current_node.y - 1;
      turnLeft();
    }
    else
    {
      goStop();
      Serial.print(3);
      Serial.print(", ");
      Serial.println(current_dir);
      back_track = true;
    }
  }
}

void backtrack()
{
  Serial.println("in backtrack");
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
    // north and if next node is to the east
    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, North head East");
      Serial.println();
      current_dir = 1;
      current_node = stack.pop();
      back_track = false;
      turnRight();
    }
    // north and if next node is to the west
    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, North head West");
      Serial.println();
      current_dir = 3;
      current_node = stack.pop();
      back_track = false;
      turnLeft();
    }
    // north and if next node is to the south
    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)
    {
      Serial.println ("BackTrack, North head South");
      Serial.println();
      current_dir = 2;
      current_node = stack.pop();
      back_track = false;
      uTurn();
    }
  }
  //east
  else if (current_dir == 1)
  {
    // east and if next node is north
    if (current_node.x == next_node.x && current_node.y == next_node.y - 1)
    {
      Serial.println ("BackTrack, East head North");
      Serial.println();
      current_dir = 0;
      current_node = stack.pop();
      back_track = false;
      turnLeft();
    }
    // east and if next node is to the east
    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, East head East");
      Serial.println();
      current_dir = 1;
      current_node = stack.pop();
      back_track = false;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(200);
    }
    // east and if next node is to the west
    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, East head West");
      Serial.println();
      current_dir = 3;
      current_node = stack.pop();
      back_track = false;
      uTurn();
    }
    // east and if next node is to the south
    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)
    {
      Serial.println ("BackTrack, East head South");
      Serial.println();
      current_dir = 2;
      current_node = stack.pop();
      back_track = false;
      turnRight();
    }
  }
  //south
  else if (current_dir == 2)
  {
    // south and if next node is north
    if (current_node.x == next_node.x && current_node.y == next_node.y - 1)
    {
      Serial.println ("BackTrack, South head North");
      Serial.println();
      current_dir = 0;
      current_node = stack.pop();
      back_track = false;
      uTurn();
    }
    // south and if next node is to the east
    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, South head East");
      Serial.println();
      current_dir = 1;
      current_node = stack.pop();
      back_track = false;
      turnLeft();
    }
    // south and if next node is to the west
    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, South head West");
      Serial.println();
      current_dir = 3;
      current_node = stack.pop();
      back_track = false;
      turnRight();
    }
    // south and if next node is to the south
    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)
    {
      Serial.println ("BackTrack, South head South");
      Serial.println();
      current_dir = 2;
      current_node = stack.pop();
      back_track = false;
      digitalWrite(forPin, HIGH);
      goFor();
      delay(200);
    }
  }
  // west
  else if (current_dir == 3)
  {
    // west and if next node is north
    if (current_node.x == next_node.x && current_node.y == next_node.y - 1)
    {
      Serial.println ("BackTrack, West head North");
      Serial.println();
      current_dir = 0;
      current_node = stack.pop();
      back_track = false;
      turnRight();
    }
    // west and if next node is to the east
    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, West head East");
      Serial.println();
      current_dir = 1;
      current_node = stack.pop();
      back_track = false;
      uTurn();
    }
    // west and if next node is to the west
    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)
    {
      Serial.println ("BackTrack, West head West");
      Serial.println();
      current_dir = 3;
      current_node = stack.pop();
      back_track = false;
      goFor();
      delay(200);
    }
    // west and if next node is to the south
    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)
    {
      Serial.println ("BackTrack, West head South");
      Serial.println();
      current_dir = 2;
      current_node = stack.pop();
      back_track = false;
      digitalWrite(forPin, HIGH);
      turnLeft();
      delay(200);
    }
  }
}
bool detect(float ptValue)
{
  if (ptValue > 300)
  {
    return true;
  }
  return false;
}
void loop()
{
  digitalWrite(intersectPin, LOW);
  digitalWrite(forPin, LOW);
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
  int sensorVal[totalChannels];

  // values for line sensors
  for (int i = 0; i < 3; i++)
  {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);

    sensorVal[i] = analogRead(A0);
  }

  // values for IR sensors
  for (int i = 3; i < totalChannels; i++)
  {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);

    float volt = analogRead(A0) * 0.0048828125;
    sensorVal[i] = 13 * pow(volt, -1);
  }
  X = bitRead(6, 0);
  Y = bitRead(6, 1);
  Z = bitRead(6, 2);

  digitalWrite(addressX, X);
  digitalWrite(addressY, Y);
  digitalWrite(addressZ, Z);

  float ptValue = analogRead(A0);
  
  // line sensors
  C = sensorVal[0];
  R = sensorVal[1];
  L = sensorVal[2];

  // wall sensor distance
  distC = sensorVal[3];
  distR = sensorVal[4];
  distL = sensorVal[5];

    
  // keeping robot on the line
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
  if (detect(ptValue))
  {
    while(1)
    {
      goStop();
      delay(250);
      Serial.println("Detected Robot");
    }
  }
  //debug back_track
  // reaches intersection, make turn decision
  if (isOn(C) && isOn(R) && isOn(L))
  {
    // print currentNode
    Serial.print("Current Node: ");
    Serial.print(current_node.x);
    Serial.print(", ");
    Serial.println(current_node.y);
    if (back_track == false)
    {
      dfs();
    }
    else if (back_track == true)
    {
      backtrack();
    }
    //finished?
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
  } 
}
