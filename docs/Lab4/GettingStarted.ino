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

volatile int x = 0x0;
volatile int y = 0x0;
volatile int k = 0;
volatile int j = 0;
volatile int none = 0x0;
volatile int WD = 0x6;

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



// The role of the current running sketch

role_e role = role_pong_back;

void Data_Interpreter( volatile int info_received )
{
  int WD = 15 & info_received;
  int y =  240 & info_received;
  int x =  3840 & info_received;

  switch( WD ){
  case 1: /*Left Wall*/ printf( "Left Wall \n" ); break;
  case 2: /*Right Wall*/ printf( "Right Wall \n"); break;
  case 3: /*Left and Right Wall*/ printf(" Right Wall \n"); break;
  case 4: /*Front Wall*/ printf(" Front Wall \n"); break;
  case 5: /*Front and Left */ printf(" Front and Left Wall \n"); break;
  case 6: /*Front and Right*/ printf(" Front and Right Wall \n"); break;
  case 7: /*Front, Right and Left */ printf(" Front, Right, and Left Wall \n"); break;
  default: /*No Wall*/ printf(" No Wall ");
  }

  switch( y ){
    case 16: /*y = 1*/ printf(" y = 1 \n"); break;
    case 32: /*y = 2*/ printf(" y = 2 \n"); break;
    case 48: /*y = 3*/ printf(" y = 3 \n"); break;
    case 64: /*y = 4*/ printf(" y = 4 \n"); break;
    case 80: /*y = 5*/ printf(" y = 5 \n"); break;
    case 96: /*y = 6*/ printf(" y = 6 \n"); break;
    case 112: /*y = 7*/ printf(" y = 7 \n"); break;
    case 128: /*y = 8*/ printf(" y = 8 \n"); break;
    case 144: /*y = 9*/ printf(" y = 9 \n"); break;
    default: /*y = 0*/ printf(" y = 0 \n"); break;
  }

  switch( x ){
    case 256: /*x = 1*/ printf(" x = 1 \n"); break;
    case 512: /*x = 2*/ printf(" x = 2 \n"); break;
    case 768: /*x = 3*/ printf(" x = 3 \n"); break;
    case 1024: /*x = 4*/ printf(" x = 4 \n"); break;
    case 1280: /*x = 5*/ printf(" x = 5 \n"); break;
    case 1536: /*x = 6*/ printf(" x = 6 \n"); break;
    case 1792: /*x = 7*/ printf(" x = 7 \n"); break;
    case 2048: /*x = 8*/ printf(" x = 8 \n"); break;
    case 2304: /*x = 9*/ printf(" x = 9 \n"); break;
    default: /*x = 0*/ printf(" x = 0 \n"); break;
    
  }
  
}

void setup(void)

{
  
  //

  // Print preamble

  //


  
  
  Serial.begin(57600);
  //digitalWrite( SS, HIGH );
  //SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV8);

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
  //char c;
  //digitalWrite( SS, LOW );

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
    int data_packet = 0<<12 | x<<8 | y<<4 |WD;
    x++;
    y++;
    printf(" Data packet before sending = %x \n\r", data_packet );
    delay( 20 );
    ok = radio.write( &data_packet, 2 * sizeof( byte ) );

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
      
      volatile int info_received;
      
      bool okread;
      okread = radio.read( &info_received, 2 * sizeof( byte ) );
      printf(" info_received = %x \n\r ", info_received );
      Data_Interpreter( info_received );
      if( okread ){
        printf(" ok read \n\r ");
        radio.setAutoAck( pipes[1], true );
        //if we recorded packet, we want AutoACK on
      } else{
        printf(" didn't read \n\r ");
        radio.setAutoAck( pipes[1], false );
        //if we didn't record the packet, we don't want to receive the next packet of information into the previous node of input, so turn AutoACK off and let it continue to iterate through maze
      }
      // Spew it
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
      volatile int data_received;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.        
        done = radio.read( &data_received, 2 * sizeof( byte ) );
        // Spew it
        printf(" data received = %x \n\r ", data_received );
        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);
      }
      
      // First, stop listening so we can talk
      radio.stopListening();
      // Send the final one back.
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

}

// vim:cin:ai:sts=2 sw=2 ft=cpp
