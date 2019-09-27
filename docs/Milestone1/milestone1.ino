#include <Servo.h>

#define lightR A2
#define lightL A1
#define lightC A0

#define onButton 7

Servo servoR;
Servo servoL;

volatile int R;
volatile int L;
volatile int C;

unsigned long threshold;

void setup()
{
  // put your setup code here, to run once:
  servoR.attach(9);
  servoL.attach(10);

  pinMode(onButton, INPUT);
  pinMode(lightR, INPUT);
  pinMode(lightL, INPUT);
  pinMode(lightC, INPUT);

  Serial.begin(9600);

  //Define threshold of the Robot (Varies day to day)
  int i = 0;
  int temp = 0;
  while (i < 50)
  {
    temp += analogRead(lightC);
    i++;
  }
  threshold = temp / 50 + 300;
}

// checks if light is on the line or not (works)
boolean isOn(int light)
{
  if (light < threshold)
    return true;
  else
    return false;
}
void goFor()
{
  servoR.write(86);
  servoL.write(94);
  Serial.println("In goFor");
}

void goStop()
{
  servoR.write(90);
  servoL.write(90);
}

void slightRight()
{
  servoR.write(86);
  servoL.write(96);
}

void hardRight()
{
  servoR.write(91);
  servoL.write(100);
}

void slightLeft()
{
  servoR.write(84);
  servoL.write(94);
}

void hardLeft()
{
  servoR.write(80);
  servoL.write(89);
}
void loop()
{
  R = analogRead(lightR);
  L = analogRead(lightL);
  C = analogRead(lightC);

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
//  else if(isOn(L) && isOn(R))
//  {
//    hardRight();
//    delay(1050);
//  }
  else if (!isOn(C) && !isOn(L) && !isOn(R))
  {
    goStop();
    Serial.println("STOP");
  }
   delay(100);
}

