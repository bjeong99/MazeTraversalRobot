#define pCLK  13
#define pD0   0
#define pD1   1
#define pD2   2
#define pD3   3
#define pD4   4
#define pD5   5
#define pD6   6
#define pD7   7
#define pD8   8
#define pD9   9
#define pD10  10
#define pD11  11
#define pD12  12

void setup() {
  
  //Setup all the arduino pins to OUTPUT
  int i;
  for ( i = 0; i < 14; i ++ ){
    pinMode(i, OUTPUT);
  }

  //Start the Serial Monitor
  Serial.begin(9600);
  Serial.println("Color:");

}

void loop() {
  //Serial Data Input
  if (Serial.available() > 0){
    incomingByte = Serial.read();
    atomicParallelWrite(incomingByte);
  }
}

/* Atomic write to the parallel bus */
void parallelWrite(int n){
  noInterrupts(); //Disable system interrupts
  
  //Write the int to the serial bus
  int i;
  for(i = 0; i < 7; i++){
    digitalWrite(i, (n >> i) & 1 ? HIGH:LOW)
  }
  
  digitalWrite(pCLK, HIGH); //Data Valid High
  delay(50);
  digitalWrite(pCLK, LOW);  //Data Valid Low
  
  interrupts(); //Enable system interrupts
}
