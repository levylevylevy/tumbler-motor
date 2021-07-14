/*
  Stepper Motor Test
   
// Defin pins
 
//int reverseSwitch = 2;  // Push button for reverse
int driverPUL = 7;    // PUL- pin
int driverDIR = 6;    // DIR- pin
int spd = A0;     // Potentiometer
 
// Variables
const int stepsPerRevolution = 200; 
int pd = 500;       // Pulse Delay period
boolean setdir = LOW; // Set Direction
 
// Interrupt Handler
 
void revmotor (){
 
  setdir = !setdir;
  
}


 
void setup() {
 
  pinMode (driverPUL, OUTPUT);
  pinMode (driverDIR, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(reverseSwitch), revmotor, FALLING);
  
}
 
void loop() {
  
    pd = map((analogRead(spd)),0,1023,2000,50);
    digitalWrite(driverDIR,setdir);
    digitalWrite(driverPUL,HIGH);
    delayMicroseconds(pd);
    digitalWrite(driverPUL,LOW);
    delayMicroseconds(pd);
 
}
*/


#include <Stepper.h>

//steps = driver*4
const int stepsPerRev = 4000 *4;
int pd = 500;       // Pulse Delay period 
                                  

Stepper myStepper(stepsPerRev,2,3);    // we use pins 2,3
void setup() 
{ 
 
  myStepper.setSpeed(300); // rpm speed
  Serial.begin(9600);
}
void loop() 
{ 
  Serial.println("clockwise");
  //5 rotations per inch:
  for(int i = 0; i<10;i++) {
    myStepper.step((stepsPerRev)); 
    delay(pd);
  }

  while(1) {
    
  }
  
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRev); 
  delay(pd);

}
