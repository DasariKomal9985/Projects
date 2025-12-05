#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define SENSOR_PIN A3
#define LED_PIN 6

struct AQILevel {
  float minPM;
  float maxPM;
  int qualityNumber;
  const char* evaluation;
} AQI_TABLE[] = {
  {0,   35,   1, "Excellent"},
  {35,  79,   2, "Average"},
  {80,  115,  3, "Light Pol"},
  {115, 130,  4, "Moderate Pol"},
  {130, 200,  5, "Heavy Pol"},
  {200, 500,  6, "Serious Pol"}
};

byte barChar[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

float prevDustDensity = -1;
int prevQualityNumber = -1;
String prevEvaluation = "";
int prevBarLength = -1;

#define CHANGE_TOLERANCE 3.0

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, barChar);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  lcd.setCursor(0, 0);
  lcd.print("   Dust Sensor   ");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(280);
  int rawValue = analogRead(SENSOR_PIN);
  digitalWrite(LED_PIN, LOW);
  float voltage = (rawValue / 1023.0) * 5.0;
  float dustDensity = (voltage / 0.5) * 100.0;
  if (dustDensity < 0) dustDensity = 0;
  if (dustDensity > 500) dustDensity = 500;

  int qualityNumber = 0;
  const char* evaluation = "";
  for (int i = 0; i < 6; i++) {
    if (dustDensity >= AQI_TABLE[i].minPM && dustDensity <= AQI_TABLE[i].maxPM) {
      qualityNumber = AQI_TABLE[i].qualityNumber;
      evaluation = AQI_TABLE[i].evaluation;
      break;
    }
  }

  Serial.print("PM2.5: ");
  Serial.print(dustDensity);
  Serial.print(" ug/m3 | AQI: ");
  Serial.print(qualityNumber);
  Serial.print(" | ");
  Serial.println(evaluation);

  if (fabs(dustDensity - prevDustDensity) >= CHANGE_TOLERANCE || prevDustDensity < 0) {
    lcd.setCursor(0, 1);
    lcd.print("PM2.5:           ");
    lcd.setCursor(7, 1);
    lcd.print(dustDensity, 1);
    lcd.print(" ug/m3");
    prevDustDensity = dustDensity;
  }

  if (qualityNumber != prevQualityNumber || prevEvaluation != String(evaluation)) {
    lcd.setCursor(0, 2);
    lcd.print("AQI:             ");
    lcd.setCursor(5, 2);
    lcd.print(qualityNumber);
    lcd.print(" ");
    lcd.print(evaluation);
    prevQualityNumber = qualityNumber;
    prevEvaluation = evaluation;
  }

  int barLength = map(qualityNumber, 1, 6, 3, 20);
  if (barLength != prevBarLength) {
    lcd.setCursor(0, 3);
    for (int i = 0; i < barLength; i++) {
      lcd.write(byte(0));
    }
    for (int i = barLength; i < 20; i++) {
      lcd.print(" ");
    }
    prevBarLength = barLength;
  }

  delay(500);
}
