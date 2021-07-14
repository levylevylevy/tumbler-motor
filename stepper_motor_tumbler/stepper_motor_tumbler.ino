/*
  Stepper Motor Test
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
