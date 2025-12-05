#include <Wire.h>
#include <MFRC522_I2C.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define I2C_ADDR_RFID 0x28
#define RST_PIN -1
MFRC522_I2C mfrc522(I2C_ADDR_RFID, RST_PIN, &Wire);

#define TFT_CS 53
#define TFT_RST 8
#define TFT_DC 7
#define TFT_BL 9
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

byte validUID[4] = { 0x61, 0x2E, 0x38, 0x02 };

void setup() {
  Serial.begin(9600);
  Wire.setClock(100000);
  Wire.begin();

  tft.init(240, 320);
  tft.setRotation(1);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.fillScreen(ST77XX_BLACK);  // Clear ONCE
  tft.setTextSize(2);

  drawGateClosed();       // Left side
  drawGateClosedRight();  // Right side
  mfrc522.PCD_Init();
  delay(100);
  //tft.drawRect(175, 145, 320, 90, ST77XX_WHITE);
  tft.drawRect(0, 145, 170, 90, ST77XX_WHITE);

}


void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(300);
    return;
  }

  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uidStr += " ";
  }
  uidStr.toUpperCase();
  Serial.print("Card UID: ");
  Serial.println(uidStr);

  bool isValid = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != validUID[i]) {
      isValid = false;
      break;
    }
  }

  if (isValid) {
    Serial.println("RFID confirmed");

    // OPEN both simultaneously
    for (int offset = 0; offset <= 20; offset += 2) {
      drawGateStep(offset);       // left
      drawGateStepRight(offset);  // right
      delay(50);
    }
    drawStatusBox(190, 160, true);   // left tick
    drawStatusBox(290, 160, true);  // right tick

    delay(3000);  // wait while open

    // CLOSE both simultaneously
    for (int offset = 20; offset >= 0; offset -= 2) {
      drawGateStep(offset);       // left
      drawGateStepRight(offset);  // right
      delay(50);
    }
    drawStatusBox(190, 160, false);   // left cross
    drawStatusBox(290, 160, false);  // right cross

  } else {
    Serial.println("Wrong card!");

    drawGateDenied();
    drawGateDeniedRight();
    delay(1500);

    drawGateClosed();
    drawGateClosedRight();
  }

  mfrc522.PICC_HaltA();
}

void drawGateClosed() {
  tft.fillRect(0, 120, 80, 80, ST77XX_BLACK);               // Clear left block only
  tft.fillRect(180, 150, 40, 80, ST77XX_WHITE);              // Left terminal
  tft.fillTriangle(220, 150, 240, 150, 220, 230, ST77XX_RED);  // Flap
  drawStatusBox(190, 160, false);
}

void drawGateDenied() {
  drawStatusBox(190, 160, false);  // Cross only
}

void drawGateOpenAnimated() {
  for (int offset = 0; offset <= 20; offset += 2) {
    drawGateStep(offset);
    delay(50);
  }
  drawStatusBox(190, 160, true);  // Final tick mark
}

void drawGateCloseAnimated() {
  for (int offset = 20; offset >= 0; offset -= 2) {
    drawGateStep(offset);
    delay(50);
  }
  drawStatusBox(190, 160, false);  // Cross again
}

void drawGateStep(int offset) {
  // Clear previous flap area
  tft.fillTriangle(220, 150, 240, 150, 220, 230, ST77XX_BLACK);  // Left terminal

  // Triangle vertices for right-angled triangle (left gate)
  int topLeftX = 220;
  int topLeftY = 150;
  int topRightX = 240 - offset;
  int topRightY = 150;
  int bottomX = 220;
  int bottomY = 230;

  tft.fillTriangle(topLeftX, topLeftY, topRightX, topRightY, bottomX, bottomY, ST77XX_RED);
}

void drawStatusBox(int x, int y, bool isAllowed) {
  tft.fillRect(x, y, 20, 20, ST77XX_BLACK);  // Clear box

  if (isAllowed) {
    // Green tick
    tft.drawLine(x + 3, y + 10, x + 8, y + 15, ST77XX_GREEN);
    tft.drawLine(x + 8, y + 15, x + 17, y + 5, ST77XX_GREEN);
  } else {
    // Red X
    tft.drawLine(x + 4, y + 4, x + 16, y + 16, ST77XX_RED);
    tft.drawLine(x + 16, y + 4, x + 4, y + 16, ST77XX_RED);
  }
}

// Draw right gate closed (static)
void drawGateClosedRight() {
  tft.fillRect(280, 150, 40, 80, ST77XX_BLACK);                // Clear right block only
  tft.fillRect(280, 150, 40, 80, ST77XX_WHITE);                // Right terminal
  tft.fillTriangle(280, 150, 260, 150, 280, 230, ST77XX_RED);  // Flap
  drawStatusBox(290, 160, false);
}


// Denied state (X mark)
void drawGateDeniedRight() {
  drawStatusBox(290, 160, false);  // Cross only
}

// Animate right gate opening
void drawGateOpenAnimatedRight() {
  for (int offset = 0; offset <= 20; offset += 2) {
    drawGateStepRight(offset);
    delay(50);
  }
  drawStatusBox(290, 160, true);  // Tick mark
}

// Animate right gate closing
void drawGateCloseAnimatedRight() {
  for (int offset = 20; offset >= 0; offset -= 2) {
    drawGateStepRight(offset);
    delay(50);
  }
  drawStatusBox(290, 160, false);  // Cross again
}

// Animation step function (right side)
void drawGateStepRight(int offset) {
  // Clear previous flap area
  tft.fillTriangle(280, 150, 260, 150, 280, 230, ST77XX_BLACK);  // Right terminal

  // Triangle vertices for right-angled triangle (right gate)
  int topRightX = 280;
  int topRightY = 150;
  int topLeftX = 260 + offset;
  int topLeftY = 150;
  int bottomX = 280;
  int bottomY = 230;

  tft.fillTriangle(topRightX, topRightY, topLeftX, topLeftY, bottomX, bottomY, ST77XX_RED);
}