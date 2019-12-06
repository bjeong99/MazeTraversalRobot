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

#include "printf.h"
//Integration parts, not used for Lab 4 char array display
/*
volatile byte x_loc = 10;
volatile byte y_loc= 15;
volatile byte flags;

volatile byte k = 0;
volatile byte l = 0;

volatile byte x = 0;
volatile byte y = 0;

volatile byte i = 0;
volatile byte j = 0;

volatile byte data_packet[3] = {x_loc, y_loc, flags};
*/
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
const uint64_t pipes[2] = { 0x0000000008LL, 0x0000000009LL };
//unsigned char array used in lab 4
unsigned char maze[5][5] = 
{
  3,3,3,3,3,
  3,1,1,1,3,
  3,2,0,1,2,
  3,1,3,1,3,
  3,0,3,1,0,
};

//

// Role management

//

// Set up role.  This sketch uses the same software for all the nodes

// in this system.  Doing so greatly simplifies testing.

//



// The various roles supported by this sketch

typedef enum { role_ping_out = 1, role_pong_back } role_e;



// The debug-friendly names of those roles

const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};



// The role of the current running sketch

role_e role = role_pong_back;



void setup(void)

{
  
  //

  // Print preamble

  //


  
  
  Serial.begin(57600);
  digitalWrite( SS, HIGH );
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);

  printf_begin();

  printf("\n\rRF24/examples/GettingStarted/\n\r");

  printf("ROLE: %s\n\r",role_friendly_name[role]);

  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");



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

  radio.setPALevel(RF24_PA_LOW);

  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps

  radio.setDataRate(RF24_250KBPS);



  // optionally, reduce the payload size.  seems to

  // improve reliability

  radio.setPayloadSize( 30 );
  //25 bytes for char array, for competition, only one byte needed


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

}



void loop(void)

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
    /*
    x_loc++;
    if( x_loc %10 == 0 ){
      y_loc++;
    }
    */
    bool ok;
    
    ok = radio.write( &maze, 25*sizeof( unsigned char ) );
    //25 elements in array, each one unsigned char, so total amount of data sent is 25 bytes
    //x++; if( x%5 == 0 ){ y++; }
    
    //ok = radio.write( &maze, 25*sizeof( unsigned char ) );
    //bool ok = radio.write( &maze, 25*sizeof(unsigned char) );



    if (ok){

      printf("ok...");
      radio.setAutoAck( pipes[1], true );
      //If packet is sent, we want to recognize it and put AutoAck on

    }else{

      printf("failed.\n\r");
      radio.setAutoAck( pipes[1], false );
      //If we drop a packet, we don't want the next packet to enter the 
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

      unsigned char maze_received[5][5];
      volatile byte info_received[3];
      
     // bool okread;
      //okread = radio.read( &info_received, 3 );
      bool okread = radio.read( &maze_received[i][j], sizeof(unsigned char) );
      i++; if( i%5 == 0 ) {j++;}
      if( okread){
        printf(" ok read \n\r ");
        radio.setAutoAck( pipes[0], true );
      } else{
        printf(" didn't read \n\r ");
        radio.setAutoAck( pipes[0], false );
      }

      printf(" Got response %u \n\r ", maze_received[0][0] );
      //for( int k = 0; k < 3; k++ ){
        //printf(" %d \n\r", info_received[k] );
      //}
      // Spew it
      for( int k = 0; k < 5; k++ ){
          for( int j = 0; j < 5; j++ ){
            printf("%u, \r", maze_received[j][k] );
          }
          printf(" \n\r ");
        }
      //printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);

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
      //unsigned long got_time;
      unsigned char maze_received[5][5];

      bool done = false;

      while (!done)

      {

        // Fetch the payload, and see if this was the last one.
        
        done = radio.read( &maze_received[k][l], sizeof(unsigned char) );
        k++; if( k%5 == 0 ){ l++;}


        // Spew it

        //printf("Got payload %lu...",got_time);
        for( int k = 0; k < 5; k++ ){
          for( int j = 0; j < 5; j++ ){
            printf(" %u, ", maze_received[ j ][ k ] );
          }
          printf(" \n ");
        }


        // Delay just a little bit to let the other unit

        // make the transition to receiver

        delay(20);
      }



      // First, stop listening so we can talk

      radio.stopListening();



      // Send the final one back.

      radio.write( &maze_received, 25*sizeof(unsigned char) );

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

}

// vim:cin:ai:sts=2 sw=2 ft=cpp
