#include <Stepper.h>
#include <LiquidCrystal.h>

int mode = 0;
bool noInterrupt = false;

const int xJoystickPin = A1; // analog pin connected to X output
const int yJoystickPin = A0; // analog pin connected to Y output
const int SW_pin = 18; // digital pin connected to switch output
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//steps = driverSetSteps * 4
const int pd = 500;       // Pulse Delay period
const int limSwitch1 = 3;
int stepsPerMM = 1600; //400 * 4 / 25;
Stepper xAxis(stepsPerMM, 2, 4);
Stepper yAxis(stepsPerMM, 5, 6);
volatile bool stopCurrentMove = false;

void stepAxis(String axis, int unit) {
  if (axis.equals("x")) {
    xAxis.step(stepsPerMM * unit);
  } else {
    yAxis.step(stepsPerMM * unit);
  }
}

void moveSteps(int disX, int disY) {
  int movedX = 0;
  int movedY = 0;
  int xPerMove = 1;
  int yPerMove = 1;
  int xDir = 1;
  int yDir = 1;

  if (movedX > disX) {
    xDir = -1;
  }
  if (movedY > disY) {
    yDir = -1;
  }

  disX = abs(disX);
  disY = abs(disY);
  if (disX >= disY) {
    xPerMove = disX / disY;
  } else {
    yPerMove = disY / disX;
  }

  while ((movedX != disX || movedY != disY) && !stopCurrentMove) {
    for (int i = 0; i < xPerMove && disX != movedX; i++) {
      xAxis.step(stepsPerMM / 25 * xDir);
      movedX += 1;
    }
    for (int i = 0; i < yPerMove && disY != movedY; i++) {
      yAxis.step(stepsPerMM / 25 * yDir);
      movedY += 1;
    }
  }
}

void hitLimit() {
  if (!noInterrupt) {
    noInterrupt = true;
    Serial.println("Hit");
    stepAxis("x", -1);
    stepAxis("y", -1);
    delay(200);
    noInterrupt = true;
  }
}

void modeSwitch() {
  if (!noInterrupt) {
    noInterrupt = true;
    Serial.println("mode");
    mode = mode ^ 1;
    stopCurrentMove = true;
    delay(1000);
    noInterrupt = true;
  }
}

void setup() {
  Serial.begin(9600);
  // speed, unit unknown
  attachInterrupt(digitalPinToInterrupt(limSwitch1), hitLimit, FALLING);
  attachInterrupt(digitalPinToInterrupt(SW_pin), modeSwitch, FALLING);
  xAxis.setSpeed(350);
  yAxis.setSpeed(350);// speed, unit unknown

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  pinMode(SW_pin, INPUT);
  pinMode(yJoystickPin, INPUT);
  pinMode(xJoystickPin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
}

void loop() {
  Serial.println(mode);
  if (mode == 0) {
    moveSteps(190, 50);
    delay(500);
    moveSteps(-190, -50);
    delay(500);
  }

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

    moveSteps((xJoystick - 500) / 200, (yJoystick - 500) / 200);
  }
  stopCurrentMove = false;
  noInterrupt = false;
}
