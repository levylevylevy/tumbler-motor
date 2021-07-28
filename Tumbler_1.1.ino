#include <Stepper.h>

// xTravel = yTraval = 37.5 cm
// zTravel = 18 cm
// x,y,z -> 25 steps/cm
// holding block length = 8 cm


// Setting variables
int mode = 0;
bool noInterrupt = false;

// Joystick & display variables
const int xJoystickPin = A1; // analog pin connected to X output
const int yJoystickPin = A0; // analog pin connected to Y output
const int zDown = 8; // z digital pin for down
const int zUp = 9; // z digital pin for up
const int SW_pin = 18; // digital pin connected to switch output
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Stepper motor Variables
// Steps = driverSetSteps * 4
const int limSwitchX = 24;
const int limSwitchY = 25;
const int limSwitchZ = 26;
int stepDis = 1600; //400 * 4 / 25;
Stepper xAxis(stepDis, 2, 4);
Stepper yAxis(stepDis, 5, 6);
Stepper zAxis(stepDis, 22, 23);
volatile bool stopCurrentMove = false;

// Step the given axis in the given direction
// Does NOT have stopCurrentMove functionality but only does 1 step per call
// char axis: x, y, or Z for which axis
// int dir: 1 or -1 for which direction
void stepAxis(char axis, int dir) {
  if (axis == 'x') {
    xAxis.step(stepDis / 25 * dir);
  } else if (axis == 'y') {
    yAxis.step(stepDis / 25 * dir);
  } else {
    zAxis.step(stepDis / 25 * dir);
  }
}

// move to the given polar location in step increments
// 25 steps/cm
// Has partial stopCurrentMove functionality
// int disX: Total num of steps in the X direction
// int disY: Total num of steps in the Y direction
void moveXY(int disX, int disY) {
  int movedX = 0;
  int movedY = 0;
  int xPerMove = 1;
  int yPerMove = 1;
  int xDir = 1;
  int yDir = 1;

  // set directions
  if (movedX > disX) {
    xDir = -1;
  }
  if (movedY > disY) {
    yDir = -1;
  }
  disX = abs(disX);
  disY = abs(disY);

  // Estimate step ratio to reach the end point
  // for each axis at around the same time
  if (disX >= disY) {
    if (disY != 0) {
      xPerMove = disX / disY;
    }
  } else {
    if (disX != 0) {
      yPerMove = disY / disX;
    }
  }

  // loop until end point reached
  while ((movedX != disX || movedY != disY) && !stopCurrentMove) {
    //    // move x-axis estimated number of steps per loop or until end position reached
    //    for (int i = 0; i < xPerMove && disX != movedX; i++) {
    //      xAxis.step(stepDis / 25 * xDir);
    //      movedX += 1;
    //    }
    //    // move y-axis estimated number of steps per loop or until end position reached
    //    for (int i = 0; i < yPerMove && disY != movedY; i++) {
    //      yAxis.step(stepDis / 25 * yDir);
    //      movedY += 1;
    //    }

    // move x-axis estimated number of steps per loop or until end position reached
    for (int i = 0; i < xPerMove && disX != movedX && !stopCurrentMove; i++) {
      xAxis.step(stepDis / 25 * xDir);
      movedX += 1;
    }
    // move y-axis estimated number of steps per loop or until end position reached
    for (int i = 0; i < yPerMove && disY != movedY && !stopCurrentMove; i++) {
      yAxis.step(stepDis / 25 * yDir);
      movedY += 1;
    }
  }
}

// move the given axis in the direction for the number of steps
// Has stopCurrentMove functionality
void moveAxisSteps(char axis, int dir, int steps) {
  for (int i = 0; i < steps && !stopCurrentMove; i++) {
    stepAxis(axis, dir);
  }
}

// Move the given axis back
void hitLimit(char axis) {
  moveAxisSteps(axis, -1, 15);
  //  moveAxisSteps(axis, 1, 15);
  delay(2000);
  noInterrupt = false;
}

// Move the xAxis back after interrupt
void hitLimitX() {
  Serial.println("HitX");
  if (!noInterrupt) {
    noInterrupt = true;
    hitLimit('x');
  }
}

// Move the yAxis back after interrupt
void hitLimitY() {
  Serial.println("HitY");
  if (!noInterrupt) {
    noInterrupt = true;
    hitLimit('y');
  }
}

// Move the ZAxis back after interrupt
void hitLimitZ() {
  Serial.println("HitZ");
  if (!noInterrupt) {
    noInterrupt = true;
    hitLimit('z');
  }
}

// Change mode after interrupt
void modeSwitch() {
  if (!noInterrupt) {
    noInterrupt = true;
    Serial.println("mode");
    mode = mode ^ 1;
    stopCurrentMove = true;
    delay(2000);
    noInterrupt = false;
  }
}

// initialize Axis with limit switches and move to center
void initializeAxis() {
  xAxis.setSpeed(150);
  yAxis.setSpeed(150);
  zAxis.setSpeed(150);
  Serial.println("Initialize");
  noInterrupt = true;
  stopCurrentMove = false;
  moveXY(-20, -20);
  for (int i = 0; i < -20; i++) {
    stepAxis('z', -1);
  }

  Serial.println("X-initialize");
  while (!digitalRead(limSwitchX)) {
    stepAxis('x', 1);
  }
  xAxis.setSpeed(350);
  moveXY(-368, 0);

  Serial.println("Y-initialize");
  while (!digitalRead(limSwitchY)) {
    stepAxis('y', 1);
  }
  yAxis.setSpeed(350);
  moveXY(0, -368);

  Serial.println("Z-initialize");
  while (!digitalRead(limSwitchZ)) {
    stepAxis('z', 1);
  }
  zAxis.setSpeed(150);
  moveAxisSteps('z', -1, 125);

  delay(1000);
  noInterrupt = false;
}

// Manual Axis control mode
void manualMode() {
  int xJoystick = analogRead(xJoystickPin);
  int yJoystick = analogRead(yJoystickPin);

  // move at pseudo-variable speed based on joystick position
  moveXY((xJoystick - 414) / 200, (yJoystick - 414) / 200);
  while (digitalRead(zDown)) {
    stepAxis('z', 1);
  }
  while (digitalRead(zUp)) {
    stepAxis('z', -1);
  }
}

// Move the x and y axis in a non-adjustable circle
// and move z back and forth
void staticCircle() {
  int cirX[8] = {30, -30, -70, -70, -30, 30, 70, 70};
  int cirY[8] = {30, 30, 70, -70, -30, -30, -70, 70};

  for (int i = 0; i < 8; i++) {
    moveXY(cirX[i], cirY[i]);
  }
  for (int i = 0; i < 30; i++) {
    stepAxis('z', 1);
  }
  for (int i = 0; i < 30; i++) {
    stepAxis('z', -1);
  }
}

// Setup function
void setup() {
  Serial.begin(9600);

  // speed, unit unknown. max ~350
  xAxis.setSpeed(350);
  yAxis.setSpeed(350);
  zAxis.setSpeed(150);

  // set pinmodes
  pinMode(SW_pin, INPUT_PULLUP);
  pinMode(yJoystickPin, INPUT);
  pinMode(xJoystickPin, INPUT);
  pinMode(limSwitchX, INPUT);
  pinMode(limSwitchY, INPUT);
  pinMode(limSwitchZ, INPUT);
  pinMode(zDown, INPUT);
  pinMode(zUp, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);

  // interrupts
  //  attachInterrupt(digitalPinToInterrupt(limSwitchX), hitLimitX, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(limSwitchY), hitLimitY, FALLING);
  //  attachInterrupt(digitalPinToInterrupt(limSwitchZ), hitLimitZ, FALLING);
  attachInterrupt(digitalPinToInterrupt(SW_pin), modeSwitch, FALLING);

//  initializeAxis();
}

// loop Function
void loop() {

  // auto mode
  if (mode == 0) {
    staticCircle();
  }

  // Joystick mode
  if (mode == 1) {
    manualMode();
  }

  // reset variables for loop
  stopCurrentMove = false;
  noInterrupt = false;
}
