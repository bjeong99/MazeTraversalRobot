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

int totalChannels = 6;

int addressX = 5;
int addressY = 6;
int addressZ = 7;

int X = 0;
int Y = 0;
int Z = 0;

int distanceThreshold = 15;

int intersectPin = ?;
int leftPin = ?;
int forPin = ?;
int rightPin = ?;
void setup()
{
  // LED pin
  pinMode (intersectPin, OUTPUT);
  pinMode (leftPin, OUTPUT);
  pinMode (forPin, OUTPUT);
  pinMode (rightPin, OUTPUT);
  // Prepare address pins for output
  pinMode (addressX, OUTPUT);
  pinMode (addressY, OUTPUT);
  pinMode (addressZ, OUTPUT);

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
  while (i < 50)
  {
    temp += analogRead(A0);
    i++;
  }
  threshold = temp / 50 + 400;
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
  servoR.write(86);
  servoL.write(94);
}
void goBack()
{
  servoR.write(94);
  servoL.write(86);
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
  servoR.write(95);
  servoL.write(95);
}

void slightLeft()
{
  servoR.write(84);
  servoL.write(94);
}

void hardLeft()
{
  servoR.write(85);
  servoL.write(85);
}


// return sensor values if values needed in nested loop
int getC ()
{
    X = bitRead(0, 0);
    Y = bitRead(0, 1);
    Z = bitRead(0, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);
    
    return analogRead(A0);
}

int getR ()
{
    X = bitRead(1, 0);
    Y = bitRead(1, 1);
    Z = bitRead(1, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);
    
    return analogRead(A0);
}

int getL ()
{
    X = bitRead(2, 0);
    Y = bitRead(2, 1);
    Z = bitRead(2, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);
    
    return analogRead(A0);
}

void loop()
{
  digitalWrite(intersectPin, LOW);
  digitalWrite(forPin, LOW);
  digitalWrite(leftPin, LOW);
  digitalWrite(rightPin, LOW);
  int sensorVal[totalChannels];

  // values for line sensors
  for (int i = 0; i < 3; i++) {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);
    
    sensorVal[i] = analogRead(A0);
  }

  // values for IR sensors
  for (int i = 3; i < totalChannels; i++) {
    X = bitRead(i, 0);
    Y = bitRead(i, 1);
    Z = bitRead(i, 2);

    digitalWrite(addressX, X);
    digitalWrite(addressY, Y);
    digitalWrite(addressZ, Z);

    float volt = analogRead(A0) * 0.0048828125;
    sensorVal[i] = 13 * pow(volt, -1);
  }
  // line sensors
  C = sensorVal[0];
  R = sensorVal[1];
  L = sensorVal[2];

  // IR sensor distance
  distC = sensorVal[3];
  distR = sensorVal[4];
  distL = sensorVal[5];

  // keeping robot on the line
  if (isOn(C) && !isOn(L) && !isOn(R))
    goFor();
  else if (isOn(L) && !isOn(R)){
    slightLeft();
  }
  else if (!isOn(L) && isOn(R)){ 
    slightRight();
  }
  else if (!isOn(C) && !isOn(L) && !isOn(R)){
    goStop();
  }

  
  if (isOn(C)  && isOn(R) && isOn(L)) {
    digitalWrite(intersectPin, HIGH);
    // go forward, C'R
    if (!isWall(distC) && isWall(distR)) {
      digitalWrite(forPin, HIGH);
      goFor();
    }

    // turn right, R'
    else if(!isWall(distR)){
      digitalWrite(rightPin, HIGH);
      goFor();
      delay(750);
      hardRight();
      delay(500);
      L = getL();
      while(!isOn(L)){
        L = getL();
        Serial.println(C);
      }
      goStop();
    }

    // turn left, C`LR
    else if(isWall(distC) && !isWall(distL) && isWall(distR)) {
      digitalWrite(leftPin, HIGH);
      goFor();
      delay(750);
      hardLeft();
      delay(500);
      R = getR();
      while(!isOn(R)){
        R = getR();
        Serial.println(C);
      }
      goStop();
    }

    // turn twice, CLR
    else if(isWall(distC) && isWall(distL) && isWall(distR)) {
      digitalWrite(leftPin, HIGH);
      goFor();
      delay(750);
      hardLeft();
      delay(500);
      R = getR();
      while(!isOn(R)){
        R = getR();
        Serial.println(C);
      }
      hardLeft();
      delay(500);
       R = getR();
      while(!isOn(R)){
        R = getR();
        Serial.println(C);
      }
      goStop();
    }
  }
  delay(100);
}

