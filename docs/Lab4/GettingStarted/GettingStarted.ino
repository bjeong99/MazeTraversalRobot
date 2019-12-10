/*

 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>



 This program is free software; you can redistribute it and/or

 modify it under the terms of the GNU General Public License

 version 2 as published by the Free Software Foundation.

 */



/**

 * Example for Getting Started with nRF24L01+ radios.

 *

 * This is an example of how to use the RF24 class.  Write this sketch to two

 * different nodes.  Put one of the nodes into 'transmit' mode by connecting

 * with the serial monitor and sending a 'T'.  The ping node sends the current

 * time to the pong node, which responds by sending the value back.  The ping

 * node can then see how long the whole cycle took.

 */



#include <SPI.h>

#include "nRF24L01.h"

#include "RF24.h"

//#include <printf.h>
//Integration parts, not used for Lab 4 char array display





#define BIT0 A0 // GPIO_1_D[32]
#define BIT1 A1 // GPIO_1_D[31]
#define BIT2 A2 // GPIO_1_D[30]
#define BIT3 A3 // GPIO_1_D[29]
#define BIT4 A4 // GPIO_1_D[28]
#define BIT5 A5 // GPIO_1_D[27]
#define BIT6  2 // GPIO_1_D[26]
#define BIT7  3 // GPIO_1_D[25]
#define CLK 4 // GPIO_1_D[33]

//

// Hardware configuration

//



// Set up nRF24L01 radio on SPI bus plus pins 9 & 10



RF24 radio(9,10);



//

// Topology

//



// Radio pipe addresses for the 2 nodes to communicate.
//Calculated from equation 2(3D + N) + X[n] where X = [0,1]
const uint64_t pipes[2] = { 0x00000000088LL, 0x0000000089LL };

static int x = 0x0;
static int y = 0x0;
static int k = 0;
static int j = 0;
static int none = 0x0;
static int WD = 0x6;

static int ori; // 0 Is north
                // 2 Is South
                // 3 Is West
                // 2 Is East


//4 hex number
//int data_packet = 0x0000;

// Role management
//

// Set up role.  This sketch uses the same software for all the nodes

// in this system.  Doing so greatly simplifies testing.

//



// The various roles supported by this sketch

typedef enum { role_ping_out = 1, role_pong_back } role_e;



// The debug-friendly names of those roles

const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

static int yPrev, xPrev;

// The role of the current running sketch

role_e role = role_pong_back;

/* Send a byte to the FPGA
 * Data Format:
 *      B[7:6]: Wall Position
 *          0: Top Wall
 *          1: Right Wall
 *          2: Bottom Wall
 *          3: Left Wall
 *      B[5:3]: Y Coordinate
 *      B[2:0]: X Coordinate
 */
void parallelTransmit( int sendByte )
{

    sendByte = (0x00FF & sendByte); // Get rid of the Shit
    digitalWrite( CLK, LOW); // Assert Clock Low

    /* Write the GPIO, Keeping CLK Low */
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

    digitalWrite(CLK, HIGH);
    delay(10);
    digitalWrite(CLK, LOW);


}

void Data_Interpreter( int info_received )
{
  xPrev = x; yPrev =  y;
  WD = (0x000f & info_received) >> 0;
  y =  (0x00f0 & info_received) >> 4;
  x =  (0x0f00 & info_received) >> 8;
  ori =(0xf000 & info_received) >> 12;

  int xSend, ySend;
  int wall[4];
  memset(&wall, 0, 8);
  
  switch( WD ){
  case 1: wall[3] = 1; printf( "Left Wall \n" ); break;
  case 2: wall[1] = 1; printf( "Right Wall \n"); break;
  case 3: wall[3] = 1; wall[1] = 1; printf(" Right Wall \n"); break;
  case 4: wall[0] = 1; printf(" Front Wall \n"); break;
  case 5: wall[0] = 1; wall[3] = 1; printf(" Front and Left Wall \n"); break;
  case 6: wall[0] = 1; wall[1] = 1; printf(" Front and Right Wall \n"); break;
  case 7: wall[0] = 1; wall[1] = 1; wall[3] = 1; printf(" Front, Right, and Left Wall \n"); break;
  default: /*No Wall*/ printf(" No Wall ");
  }

  switch( y ){
    case 0: ySend = 0; printf(" y = 1 \n"); break;
    case 1: ySend = 1; printf(" y = 2 \n"); break;
    case 2: ySend = 2; printf(" y = 3 \n"); break;
    case 3: ySend = 3; printf(" y = 4 \n"); break;
    case 4: ySend = 4; printf(" y = 5 \n"); break;
    case 5: ySend = 5; printf(" y = 6 \n"); break;
    case 6:  ySend = 6; printf(" y = 7 \n"); break;
    case 7:  ySend = 7; printf(" y = 8 \n"); break;
    case 8:  ySend = 8; printf(" y = 9 \n"); break;
    default: ySend = -1; printf(" y = 0 \n"); break;
  }

  switch( x ){
    case 0: xSend = 0; printf(" x = 1 \n"); break;
    case 1: xSend = 1; printf(" x = 2 \n"); break;
    case 2: xSend = 2; printf(" x = 3 \n"); break;
    case 3: xSend = 3; printf(" x = 4 \n"); break;
    case 4: xSend = 4; printf(" x = 5 \n"); break;
    case 5: xSend = 5; printf(" x = 6 \n"); break;
    case 6: xSend = 6;  printf(" x = 7 \n"); break;
    case 7: xSend = 7; printf(" x = 8 \n"); break;
    case 8: xSend = 8; printf(" x = 9 \n"); break;
    default: xSend = -1; printf(" x = 0 \n"); break;
    
  }

  int j;
  int buff[4];
  Serial.print(ori);
  switch(ori){
        case 0: buff[0] = wall[0]; buff[1] = wall[1]; buff[2] = wall[2]; buff[3] = wall[3];
        case 1: buff[0] = wall[3]; buff[1] = wall[0]; buff[2] = wall[1]; buff[3] = wall[2];
        case 2: buff[0] = wall[2]; buff[1] = wall[3]; buff[2] = wall[0]; buff[3] = wall[1];
        case 3: buff[0] = wall[1]; buff[1] = wall[2]; buff[2] = wall[3]; buff[3] = wall[0];
  }
  
  for (int j = 0; j < 4; j++){
    if (xSend == -1 || ySend == -1) break;
      if (buff[j] == 1){
        int sendByte;
        sendByte = (j << 6) + ((xSend) << 3) + (8 - ySend);
        Serial.print(sendByte);
        parallelTransmit(sendByte);
      }
  }
  
}

void setup(void)

{
  
  //

  // Print preamble

  //
  
    pinMode(BIT0 , OUTPUT); digitalWrite(BIT0 , LOW);
    pinMode(BIT1 , OUTPUT); digitalWrite(BIT1 , LOW);
    pinMode(BIT2 , OUTPUT); digitalWrite(BIT2 , LOW);
    pinMode(BIT3 , OUTPUT); digitalWrite(BIT3 , LOW);
    pinMode(BIT4 , OUTPUT); digitalWrite(BIT4 , LOW);
    pinMode(BIT5 , OUTPUT); digitalWrite(BIT5 , LOW);
    pinMode(BIT6 , OUTPUT); digitalWrite(BIT6 , LOW);
    pinMode(BIT7 , OUTPUT); digitalWrite(BIT7 , LOW);
    pinMode(CLK, OUTPUT); digitalWrite(CLK, LOW);

    
  
  
  Serial.begin(57600);
  //digitalWrite( SS, HIGH );
  //SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV8);

  //printf_begin();

  //printf("\n\rRF24/examples/GettingStarted/\n\r");

  //printf("ROLE: %s\n\r",role_friendly_name[role]);

  //printf("*** PRESS 'T' to begin transmitting to the other node\n\r");



  //

  // Setup and configure rf radio

  //



  radio.begin();


  // optionally, increase the delay between retries & # of retries

  radio.setRetries( 50,200 );

  radio.setAutoAck(true);

  // set the channel

  radio.setChannel(0x50);

  // set the power

  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.

  radio.setPALevel( RF24_PA_HIGH );

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



   
// Become the primary receiver (pong back)

      role = role_pong_back;

      radio.openWritingPipe(pipes[1]);

      radio.openReadingPipe(1,pipes[0]);


 



  //

  // Start listening

  //



  radio.startListening();



  //

  // Dump the configuration of the rf unit for debugging

  //



  radio.printDetails();

}



void loop(void)

{
  

  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //
  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      volatile int data_received;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.        
        done = radio.read( &data_received, 2 * sizeof( byte ) );
        Data_Interpreter(data_received);
        
        delay(20);
      }
      
      // First, stop listening so we can tal
      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }



  //

  // Change roles

  //



 
     
   

}




// vim:cin:ai:sts=2 sw=2 ft=cpp
