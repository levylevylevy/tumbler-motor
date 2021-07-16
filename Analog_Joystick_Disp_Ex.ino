// include the library code:
#include <LiquidCrystal.h>

const int X_pin = A1; // analog pin connected to X output
const int Y_pin = A0; // analog pin connected to Y output
const int SW_pin = 2; // digital pin connected to switch output
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  pinMode(SW_pin, INPUT);
  pinMode(Y_pin, INPUT);
  pinMode(X_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Switch: ");
  lcd.print(digitalRead(SW_pin));
  lcd.setCursor(0, 1);
  lcd.print("X:");
  lcd.print(analogRead(X_pin));
  lcd.setCursor(8, 1);
  lcd.print("Y:");
  lcd.println(analogRead(Y_pin));
  lcd.print("   ");

//  Serial.print("Switch:  ");
//  Serial.print(digitalRead(SW_pin));
//  Serial.print("\n");
//  Serial.print("X-axis: ");
//  Serial.print(analogRead(X_pin));
//  Serial.print("\n");
//  Serial.print("Y-axis: ");
//  Serial.println(analogRead(Y_pin));
//  Serial.print("\n\n");
  delay(1000);
}
