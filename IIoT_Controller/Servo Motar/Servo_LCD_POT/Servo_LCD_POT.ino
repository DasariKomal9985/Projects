#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myServo;

const int potPin = A8;
const int servoPin = 13;
const int buttonPin = 46;

int lastAngle = -1;
String lastDirection = "---";
bool servoEnabled = false;
int lastButtonState = HIGH;

void setup() {
  lcd.init();
  lcd.backlight();
  myServo.attach(servoPin);
  pinMode(buttonPin, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  lcd.print("Servo Control");
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    servoEnabled = !servoEnabled;
    delay(200);  
  }
  lastButtonState = buttonState;

  lcd.setCursor(0, 1);
  lcd.print("Servo Status: ");
  lcd.print(servoEnabled ? "ON " : "OFF");

  if (servoEnabled) {
    int potValue = analogRead(potPin);
    int angle = map(potValue, 0, 1023, 0, 180);
    angle = constrain(angle, 0, 180);

    String direction = "---";
    if (angle > lastAngle) {
      direction = "Clock";
    } else if (angle < lastAngle) {
      direction = "Anti";
    } else {
      direction = lastDirection;
    }

    if (abs(angle - lastAngle) >= 2) {
      myServo.write(angle);
      lastAngle = angle;
      lastDirection = direction;

      lcd.setCursor(0, 2);
      lcd.print("Set : ");
      lcd.print(angle);
      lcd.print((char)223);
      lcd.print(" ");
      lcd.print(direction);
      lcd.print("     ");

      lcd.setCursor(0, 3);
      int barLength = map(angle, 0, 180, 0, 20);
      for (int i = 0; i < 20; i++) {
        if (i < barLength)
          lcd.write(255);
        else
          lcd.print(" ");
      }
    }
  }

  delay(50);
}
