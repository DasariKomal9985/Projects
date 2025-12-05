#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// TFT pin definitions
#define TFT_CS 53
#define TFT_RST 8
#define TFT_DC 7
#define TFT_BL 9

// Servo and control pins
#define SERVO_PIN 13
#define POT_PIN A8
#define BUTTON_PIN 46

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Servo myServo;

bool servoEnabled = false;
int lastButtonState = HIGH;
int lastAngle = -1;
int lastNeedleAngle = -1;
String lastDirection = "---";

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  myServo.attach(SERVO_PIN);
  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  drawStaticUI();
  updateStatusText();
  drawSpeedometerBase();  // Static gauge
  drawNeedle(0);          // Initial needle
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  if (buttonState == LOW && lastButtonState == HIGH) {
    servoEnabled = !servoEnabled;
    updateStatusText();
    delay(200);
  }
  lastButtonState = buttonState;

  if (servoEnabled) {
    int potValue = analogRead(POT_PIN);
    int angle = map(potValue, 0, 1023, 0, 180);
    angle = constrain(angle, 0, 180);

    String direction = lastDirection;
    if (angle > lastAngle) direction = "Clock";
    else if (angle < lastAngle) direction = "Anti";

    if (abs(angle - lastAngle) >= 2) {
      myServo.write(angle);
      lastAngle = angle;
      lastDirection = direction;

      updateAngleText(angle, direction);
      drawNeedle(angle);
    }
  }

  delay(50);
}

void drawStaticUI() {
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  // Title
  tft.fillRect(0, 0, 320, 30, ST77XX_BLUE);
  tft.setCursor(60, 5);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Servo Control");

  // Labels
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_YELLOW);
  tft.print("Status:");

  tft.setCursor(10, 90);
  tft.setTextColor(ST77XX_CYAN);
  tft.print("Angle:");

  tft.setCursor(10, 130);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print("Direction:");
}

void updateStatusText() {
  tft.fillRect(110, 50, 120, 25, ST77XX_BLACK);
  tft.setCursor(110, 50);
  tft.setTextSize(2);
  tft.setTextColor(servoEnabled ? ST77XX_GREEN : ST77XX_RED);
  tft.print(servoEnabled ? "ON " : "OFF");
}

void updateAngleText(int angle, String direction) {
  // Angle
  tft.fillRect(110, 90, 100, 25, ST77XX_BLACK);
  tft.setCursor(110, 90);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_CYAN);
  tft.print(angle);
  tft.print((char)223);  // degree symbol

  // Direction value (moved below label)
  tft.fillRect(10, 160, 100, 25, ST77XX_BLACK);
  tft.setCursor(10, 160);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setTextSize(2);
  tft.print(direction);
}

// Moved the speedometer to right side
void drawSpeedometerBase() {
  int cx = 210, cy = 220, r = 100;  // Shifted to the right

  // Semicircle
  tft.drawCircle(cx, cy, r, ST77XX_WHITE);
  tft.drawCircle(cx, cy, r - 1, ST77XX_WHITE);
  tft.drawCircle(cx, cy, r + 1, ST77XX_WHITE);

  // Tick marks and labels
  for (int a = 0; a <= 180; a += 30) {
    float rad = radians(180 - a);
    int x0 = cx + (r - 10) * cos(rad);
    int y0 = cy - (r - 10) * sin(rad);
    int x1 = cx + r * cos(rad);
    int y1 = cy - r * sin(rad);
    tft.drawLine(x0, y0, x1, y1, ST77XX_WHITE);

    int lx = cx + (r - 32) * cos(rad);
    int ly = cy - (r - 32) * sin(rad);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(lx - 8, ly - 6);
    tft.print(a);
  }

  // Center hub
  tft.fillCircle(cx, cy, 10, ST77XX_WHITE);
  tft.fillCircle(cx, cy, 4, ST77XX_BLACK);
}

void drawNeedle(int angle) {
  if (angle == lastNeedleAngle) return;

  int cx = 210, cy = 220, r = 100;

  // Erase old needle
  if (lastNeedleAngle >= 0) {
    float oldRad = radians(180 - lastNeedleAngle);
    int ox = cx + (r - 20) * cos(oldRad);
    int oy = cy - (r - 20) * sin(oldRad);
    int ox2 = cx + (r - 60) * cos(oldRad + 0.04);
    int oy2 = cy - (r - 60) * sin(oldRad + 0.04);
    int ox3 = cx + (r - 60) * cos(oldRad - 0.04);
    int oy3 = cy - (r - 60) * sin(oldRad - 0.04);
    tft.fillTriangle(cx, cy, ox2, oy2, ox, oy, ST77XX_BLACK);
    tft.fillTriangle(cx, cy, ox3, oy3, ox, oy, ST77XX_BLACK);

    // Redraw center hub after erase
    tft.fillCircle(cx, cy, 10, ST77XX_WHITE);
    tft.fillCircle(cx, cy, 4, ST77XX_BLACK);

    // 🔁 Redraw tick labels (in case they were erased)
    for (int a = 0; a <= 180; a += 30) {
      float rad = radians(180 - a);
      int lx = cx + (r - 32) * cos(rad);
      int ly = cy - (r - 32) * sin(rad);
      tft.setTextColor(ST77XX_YELLOW);
      tft.setTextSize(1);
      tft.setCursor(lx - 8, ly - 6);
      tft.print(a);
    }
  }

  // Draw new needle
  float rad = radians(180 - angle);
  int nx = cx + (r - 20) * cos(rad);
  int ny = cy - (r - 20) * sin(rad);
  int nx2 = cx + (r - 60) * cos(rad + 0.04);
  int ny2 = cy - (r - 60) * sin(rad + 0.04);
  int nx3 = cx + (r - 60) * cos(rad - 0.04);
  int ny3 = cy - (r - 60) * sin(rad - 0.04);

  tft.fillTriangle(cx, cy, nx2, ny2, nx, ny, ST77XX_RED);
  tft.fillTriangle(cx, cy, nx3, ny3, nx, ny, ST77XX_RED);

  // Center hub
  tft.fillCircle(cx, cy, 10, ST77XX_WHITE);
  tft.fillCircle(cx, cy, 4, ST77XX_BLACK);

  // Angle value below speedometer
  tft.fillRect(cx - 30, cy + 30, 60, 25, ST77XX_BLACK);
  tft.setTextColor(ST77XX_CYAN);
  tft.setTextSize(2);
  tft.setCursor(cx - 22, cy + 30);
  tft.print(angle);
  tft.print((char)223);

  lastNeedleAngle = angle;
}
