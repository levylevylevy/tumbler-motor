#include <Stepper.h>
#include <LiquidCrystal.h>

// Setting variables
int mode = 0;
bool noInterrupt = false;

// Joystick & display variables
const int xJoystickPin = A1; // analog pin connected to X output
const int yJoystickPin = A0; // analog pin connected to Y output
const int SW_pin = 18; // digital pin connected to switch output
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Stepper motor Variables
// Steps = driverSetSteps * 4
const int limSwitch1 = 3;
int stepsPerMM = 1600; //400 * 4 / 25;
Stepper xAxis(stepsPerMM, 2, 4);
Stepper yAxis(stepsPerMM, 5, 6);
volatile bool stopCurrentMove = false;

// Step the given axis in the given direction
// String axis: x or y for which axis
// int unit: 1 or -1 for which direction
void stepAxis(String axis, int unit) {
  if (axis.equals("x")) {
    xAxis.step(stepsPerMM * unit);
  } else {
    yAxis.step(stepsPerMM * unit);
  }
}

// move to the given polar location in step increments
// int disX: Total num of steps in the X direction
// int disY: Total num of steps in the Y direction
void moveSteps(int disX, int disY) {
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
    xPerMove = disX / disY;
  } else {
    yPerMove = disY / disX;
  }

  // loop until end point reached
  while ((movedX != disX || movedY != disY) && !stopCurrentMove) {
    // move x-axis estimated number of steps per loop or until end position reached
    for (int i = 0; i < xPerMove && disX != movedX; i++) {
      xAxis.step(stepsPerMM / 25 * xDir);
      movedX += 1;
    }
    // move y-axis estimated number of steps per loop or until end position reached
    for (int i = 0; i < yPerMove && disY != movedY; i++) {
      yAxis.step(stepsPerMM / 25 * yDir);
      movedY += 1;
    }
  }
}

// Move the axis back 1 step after interrupt
void hitLimit() {
  if (!noInterrupt) {
    noInterrupt = true;
    Serial.println("Hit");
    stepAxis("x", -1);
    stepAxis("y", -1);
    delay(200);
    noInterrupt = false;
  }
}

// Change mode after interrupt
void modeSwitch() {
  if (!noInterrupt) {
    noInterrupt = true;
    Serial.println("mode");
    mode = mode ^ 1;
    stopCurrentMove = true;
    delay(1000);
    noInterrupt = false;
  }
}


void setup() {
  Serial.begin(9600);

  // interrupts
  attachInterrupt(digitalPinToInterrupt(limSwitch1), hitLimit, FALLING);
  attachInterrupt(digitalPinToInterrupt(SW_pin), modeSwitch, FALLING);

  // speed, unit unknown. max ~350
  xAxis.setSpeed(350);
  yAxis.setSpeed(350);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // set pinmodes
  pinMode(SW_pin, INPUT);
  pinMode(yJoystickPin, INPUT);
  pinMode(xJoystickPin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
}

void loop() {
  Serial.println(mode);

  // auto mode
  if (mode == 0) {
    int cirX[8] = {30, -30, -70, -70, -30, 30, 70, 70};
    int cirY[8] = {30, 30, 70, -70, -30, -30, -70, 70};
    
    for(int i = 0; i < 8; i++) {
      moveSteps(cirX[i], cirY[i]);
    }
  }

  // Joystick mode
  if (mode == 1) {
    int xJoystick = analogRead(xJoystickPin);
    int yJoystick = analogRead(yJoystickPin);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("X:");
    lcd.print(xJoystick);
    lcd.setCursor(0, 1);
    lcd.print("Y:");
    lcd.println(yJoystick);

    // move at variable speed based on joystick position
    moveSteps((xJoystick - 414) / 200, (yJoystick - 414) / 200);
  }

  // reset variables for loop
  stopCurrentMove = false;
  noInterrupt = false;
}
