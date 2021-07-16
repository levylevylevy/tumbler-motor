#include <Stepper.h>                                  

//steps = driverSetSteps * 4
const int stepsPerRev = 4000 * 4;
const int pd = 500;       // Pulse Delay period 
volatile bool stopCurrentMove = false;

Stepper xAxis(stepsPerRev,2,4);    // we use pins 2,4 for dir, pul
Stepper yAxis(stepsPerRev,5,6);    // we use pins 5,6 for dir, pul
const int limSwitch1 = 3;

void hitLimit() {
  Serial.println("Hit");
  xAxis.step(stepsPerRev);
  yAxis.step(stepsPerRev);
  stopCurrentMove = true;
  delay(200);
}

void setup() { 
  Serial.begin(9600);
  xAxis.setSpeed(600);
  yAxis.setSpeed(600);// speed, unit unknown
  attachInterrupt(digitalPinToInterrupt(limSwitch1), hitLimit, FALLING);
}

void loop() { 
//  if (digitalRead(limSwitch1)) {
//    Serial.println("Hit");
//    myStepper.step(-stepsPerRev);
//    stopCurrentMove = true;
//    delay(200);
//  }

//  //5 rotations per inch:
  for(int i = 0; i<100 && !stopCurrentMove; i++) {
    Serial.println("clockwise");
    xAxis.step(stepsPerRev/100); 
    yAxis.step(stepsPerRev/100); 
  }

  delay(1000);
  
   for(int i = 0; i<100 && !stopCurrentMove; i++) {
    Serial.println("clockwise");
    xAxis.step(-stepsPerRev/100); 
    yAxis.step(-stepsPerRev/100); 
  }

//  while(1) {}

  delay(1000);
  stopCurrentMove = false;
  

}
