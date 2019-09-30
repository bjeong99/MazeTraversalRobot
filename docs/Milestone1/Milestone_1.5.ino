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

int k = 1;

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
void goBack()
{
  servoR.write(94);
  servoL.write(86);
  Serial.println("In goBack");
}

void goStop()
{
  servoR.write(90);
  servoL.write(90);
}

void slightRight()
{
  servoR.write(87);
  servoL.write(96);
}

void hardRight()
{
  servoR.write(91);
  servoL.write(95);
}

void slightLeft()
{
  servoR.write(84);
  servoL.write(93);
}

void hardLeft()
{
  servoR.write(85);
  servoL.write(89);
  Serial.println("Hard Left");
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
    else if (!isOn(C) && !isOn(L) && !isOn(R))
  {
    goStop();
    Serial.println("In goStop");
  }

  switch(k) {
     case 1:
      if (isOn(C) && isOn(L) && isOn(R)){
        hardLeft();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(R)) R = analogRead(lightR);
        while(!isOn(R)) R = analogRead(lightR);
        goStop();
        k++;
      }
        break;
      case 2:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardRight();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(L)) L = analogRead(lightL);
        while(!isOn(L)) L = analogRead(lightL);
        goStop();
        k++;
       }
        break;
      case 3:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardRight();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(L)) L = analogRead(lightL);
        while(!isOn(L)) L = analogRead(lightL);
        goStop();
        k++;
       }
        break;
      case 4:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardRight();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(L)) L = analogRead(lightL);
        while(!isOn(L)) L = analogRead(lightL);
        goStop();
        k++;
       }
        break;
      case 5:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardRight();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(L)) L = analogRead(lightL);
        while(!isOn(L)) L = analogRead(lightL);
        goStop();
        k++;
       }
        break;        
      case 6:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardLeft();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(R)) R = analogRead(lightR);
        while(!isOn(R)) R = analogRead(lightR);
        goStop();
        k++;
       }
        break;
      case 7:
       if (isOn(C) && isOn(L) && isOn(R)){
        hardLeft();
        while(isOn(C)) C = analogRead(lightC);
        while(isOn(R)) R = analogRead(lightR);
        while(!isOn(R)) R = analogRead(lightR);
        goStop();
        k++;
       }
        break;
  }
               

    
//  else if(isOn(L) && isOn(R))
//  {
//    hardRight();
//    delay(1050);
//  }
   delay(100);
  
}
