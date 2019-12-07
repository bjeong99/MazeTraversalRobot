#include <StackArray.h>

#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"



//#define lightR A2

//#define lightL A1

//#define lightC A0



#define onButton 7
/*////////////////////////////////////////////////RADIO GLOBAL/////////////////////////////////////////////////////////////////////*/
//Set Arduino pins 9 and 10 as our pipes
RF24 radio(9,10);
//Calculated from equation 2(3D + N) + X[n] where X = [0,1]
const uint64_t pipes[2] = { 0x00000000088LL, 0x0000000089LL };

int x = 0x0;
int y = 0x0;
int k = 0;
int j = 0;
int none = 0x0;

//4 hex number
int data_packet = 0x0000;

typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;
/*////////////////////////////////////////////////RADIO GLOBAL/////////////////////////////////////////////////////////////////////*/
Servo servoR;

Servo servoL;



volatile int R;

volatile int L;

volatile int C;



volatile int distR;

volatile int distL;

volatile int distC;

volatile int WD;

volatile int Data_Send;

int get_Wall_Detect( volatile int distR, volatile int distL, volatile int distC )
{
  if( isWall( distL ) && !isWall( distR ) && !isWall( distC ) ){
    return 1; //detect a left wall
  } else if( !isWall( distL ) && isWall( distR ) && isWall( distC ) ){
    return 2; //detect a right wall
  } else if( isWall( distL ) && isWall( distR ) && !isWall( distC ) ){
    return 3; //left wall and right wall
  } else if( !isWall( distL ) && !isWall( distR ) && isWall( distC ) ){
    return 4; //detect a center wall
  } else if( isWall( distL ) && !isWall( distR ) && isWall( distC ) ){
    return 5; //detect a center wall and left wall
  } else if( !isWall( distL ) && isWall( distR ) && isWall( distC ) ){
    return 6; //detect a center wall and right wall
  } else if( isWall( distL ) && isWall( distR ) && isWall( distC ) ){
    return 7; //detect a wall in all three directions
  } else{
    return 0; //no walls detected in any location
  }
}


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



float ptValue = 0;



bool is_in_backtrack = false;



node previous_node;



void setup()

{

  // set back track to false
  /*////////////////////////////////////////////////RADIO SETUP/////////////////////////////////////////////////////////////////////*/
  //Serial.begin(57600);
  digitalWrite( SS, HIGH );
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  printf_begin();

  printf("\n\rRF24/examples/GettingStarted/\n\r");

  printf("ROLE: %s\n\r",role_friendly_name[role]);

  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

   radio.begin();


  // optionally, increase the delay between retries & # of retries

  radio.setRetries( 50,200 );

  radio.setAutoAck(true);

  // set the channel

  radio.setChannel(0x50);

  // set the power

  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.

  radio.setPALevel(RF24_PA_LOW);

  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps

  radio.setDataRate(RF24_250KBPS);



  // optionally, reduce the payload size.  seems to

  // improve reliability

  radio.setPayloadSize( 2 );
  //

  // Open pipes to other nodes for communication

  //



  // This simple sketch opens two pipes for these two nodes to communicate

  // back and forth.

  // Open 'our' pipe for writing

  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)



  if ( role == role_ping_out )

  {

    radio.openWritingPipe(pipes[0]);

    radio.openReadingPipe( 1 , pipes[1] );

  }

  else

  {

    radio.openWritingPipe(pipes[1]);

    radio.openReadingPipe(1,pipes[0]);

  }



  //

  // Start listening

  //



  radio.startListening();



  //

  // Dump the configuration of the rf unit for debugging

  //



  radio.printDetails();
  /*////////////////////////////////////////////////RADIO SETUP/////////////////////////////////////////////////////////////////////*/

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

  if (x >= 0 && x < columns - 1 && y >= 0 && y < rows - 1)

    true;

  else

    false;

}

/* 

How DFS works:

- will only be called on intersections

- it will stop momentarily at the intersection and the current_node will be visited

- detects free tile, will update current_dir, push current_node onto stack for 

back track, and updates current_node to the tile it will travel to and heads in that 

direction

- if it needs to backtrack, current_node won't be updated, won't be pushed, and back_track will be 

set to true 

- is_in_backtrack is helpful and will make robot detection code easier

  - basically, the value is sustained when the robot travels between nodes, and can only update at intersections

  - false if travelling because of exploration, true if travelling because of backtrack

 */

void dfs()

{

  is_in_backtrack = false;

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

      WD = get_Wall_Detect( distR, distL, distC );

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



/* 

How Backtrack works:

- peeks at the next node on the stack = next_node

- compares which way to turn to get to that next_node from current_node, updates current_dir, and

pops the next_node it will go to from the stack

- back_track can only switch to true at intersection since back_track is toggled by dfs which

is only called at intersections

- is_in_backtrack is there to make robot detection code easier

  - if the robot is traveling because of the BT function, it will remain true until it hits another intersection

  - it will then turn false because DFS will be called, but will turn true again if BT needs to be continued

 */

void backtrack()

{

  is_in_backtrack = true;

  Serial.println("in backtrack");

  node next_node = stack.peek();

  previous_node = current_node;

  //north

  if (current_dir == 0)

  {

    // north and if next node is north

    if (current_node.x == next_node.x && current_node.y == next_node.y - 1)

    {

      Serial.println("BackTrack, North head North");

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

      Serial.println("BackTrack, North head East");

      Serial.println();

      current_dir = 1;

      current_node = stack.pop();

      back_track = false;

      turnRight();

    }

    // north and if next node is to the west

    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, North head West");

      Serial.println();

      current_dir = 3;

      current_node = stack.pop();

      back_track = false;

      turnLeft();

    }

    // north and if next node is to the south

    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)

    {

      Serial.println("BackTrack, North head South");

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

      Serial.println("BackTrack, East head North");

      Serial.println();

      current_dir = 0;

      current_node = stack.pop();

      back_track = false;

      turnLeft();

    }

    // east and if next node is to the east

    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, East head East");

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

      Serial.println("BackTrack, East head West");

      Serial.println();

      current_dir = 3;

      current_node = stack.pop();

      back_track = false;

      uTurn();

    }

    // east and if next node is to the south

    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)

    {

      Serial.println("BackTrack, East head South");

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

      Serial.println("BackTrack, South head North");

      Serial.println();

      current_dir = 0;

      current_node = stack.pop();

      back_track = false;

      uTurn();

    }

    // south and if next node is to the east

    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, South head East");

      Serial.println();

      current_dir = 1;

      current_node = stack.pop();

      back_track = false;

      turnLeft();

    }

    // south and if next node is to the west

    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, South head West");

      Serial.println();

      current_dir = 3;

      current_node = stack.pop();

      back_track = false;

      turnRight();

    }

    // south and if next node is to the south

    else if (current_node.x == next_node.x && current_node.y == next_node.y + 1)

    {

      Serial.println("BackTrack, South head South");

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

      Serial.println("BackTrack, West head North");

      Serial.println();

      current_dir = 0;

      current_node = stack.pop();

      back_track = false;

      turnRight();

    }

    // west and if next node is to the east

    else if (current_node.x == next_node.x - 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, West head East");

      Serial.println();

      current_dir = 1;

      current_node = stack.pop();

      back_track = false;

      uTurn();

    }

    // west and if next node is to the west

    else if (current_node.x == next_node.x + 1 && current_node.y == next_node.y)

    {

      Serial.println("BackTrack, West head West");

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

      Serial.println("BackTrack, West head South");

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

// true if robot detected, false otherwise

bool detect(float ptValue)

{

  if (ptValue > 300)

  {

    return true;

  }

  return false;

}

void loop()
/*////////////////////////////////////////////////////////RADIO LOOP////////////////////////////////////////////////////////////////*/
{
  char c;
  digitalWrite( SS, LOW );

  //

  // Ping out role.  Repeatedly send the current time

  //



  if (role == role_ping_out)

  {

    // First, stop listening so we can talk.

    radio.stopListening();



    // Take the time, and send it.  This will block until complete

    unsigned long time = millis();

    printf("Now sending %lu...",time);
    bool ok;
    //Send a two byte data packet
    data_packet = 0 << 12 | x << 8 | y << 4 | WD;
    printf(" Data packet before sending = %x \n\r", data_packet );
    delay( 20 );
    ok = radio.write( data_packet, 2 * sizeof( byte ) );



    if (ok){

      printf("ok...");
      radio.setAutoAck( true );
      //If packet is sent, we want to recognize it and put AutoAck on

    }else{

      printf("failed.\n\r");
      radio.setAutoAck( false );
      //If we drop a packet, we don't want to resend data which will go to the next input of the receiver
    }


    // Now, continue listening

    radio.startListening();



    // Wait here until we get a response, or timeout (250ms)

    unsigned long started_waiting_at = millis();

    bool timeout = false;

    while ( ! radio.available() && ! timeout )

      if (millis() - started_waiting_at > 200 )

        timeout = true;



    // Describe the results

    if ( timeout )

    {

      printf("Failed, response timed out.\n\r");
      //radio.setAutoAck( false );

    }

    else

    {
      //radio.setAutoAck( true );
      // Grab the response, compare, and send to debugging spew
      
      int info_received = 0x0000;
      
      bool okread;
      okread = radio.read( &info_received, 2 * sizeof( byte ) );

      if( okread){
        printf(" ok read \n\r ");
        radio.setAutoAck( pipes[1], true );
        //if we recorded packet, we want AutoACK on
      } else{
        printf(" didn't read \n\r ");
        radio.setAutoAck( pipes[1], false );
        //if we didn't record the packet, we don't want to receive the next packet of information into the previous node of input, so turn AutoACK off and let it continue to iterate through maze
      }

     // printf(" Got response %u \n\r ", maze_received[0][0] );
      //for( int k = 0; k < 3; k++ ){
        //printf(" %d \n\r", info_received[k] );
      //}
      // Spew it
      
      printf(" info_received = %x \n\r ", info_received );

    }



    // Try again 1s later

    delay(1000);

  }



  //

  // Pong back role.  Receive each packet, dump it out, and send it back

  //



  if ( role == role_pong_back )

  {

    // if there is data ready

    if ( radio.available() )

    {

      // Dump the payloads until we've gotten everything

      bool done = false;

      while (!done)

      {

        // Fetch the payload, and see if this was the last one.
        
        done = radio.read( &data_packet, 2 * sizeof( byte ) );
        


        // Spew it
        printf(" data received = %x \n\r ", data_packet );

        // Delay just a little bit to let the other unit

        // make the transition to receiver

        delay(20);
      }



      // First, stop listening so we can talk

      radio.stopListening();



      // Send the final one back.
      volatile byte data_received;

      radio.write( &data_received, 2 * sizeof( byte ) );

      printf("Sent response.\n\r");



      // Now, resume listening so we catch the next packets.

      radio.startListening();

    }

  }



  //

  // Change roles

  //



  if ( Serial.available() )

  {

    char c = toupper(Serial.read());

    if ( c == 'T' && role == role_pong_back )

    {

      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");



      // Become the primary transmitter (ping out)

      role = role_ping_out;

      radio.openWritingPipe(pipes[0]);

      radio.openReadingPipe(1,pipes[1]);

    }

    else if ( c == 'R' && role == role_ping_out )

    {

      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");



      // Become the primary receiver (pong back)

      role = role_pong_back;

      radio.openWritingPipe(pipes[1]);

      radio.openReadingPipe(1,pipes[0]);

    }

  }

/*////////////////////////////////////////////////////////RADIO LOOP////////////////////////////////////////////////////////////////*//*////////////////////////////////////////////////////////RADIO LOOP////////////////////////////////////////////////////////////////*/
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



  ptValue = analogRead(A0);



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

    current_dir = (current_dir + 2) % 4;

    if (is_in_backtrack == false)

    {

      current_node = stack.pop();

    }

    else

    {

      stack.push(current_node);

      current_node = previous_node;

    }

    uTurn();

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

      while (isOn(C) && isOn(R) && isOn(L))

      {

        goStop();

        Serial.println("Done");

      }

    }

  }

}
