#include <Wire.h>
#include <MFRC522_I2C.h>

#define I2C_ADDR 0x28     // Default I2C address of your RFID module
#define RST_PIN  -1       // No RST pin used in I2C mode
MFRC522_I2C mfrc522(I2C_ADDR, RST_PIN, &Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  mfrc522.PCD_Init();
  Serial.println("Scan an RFID card...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(i < mfrc522.uid.size - 1 ? " " : "\n");
  }

  mfrc522.PICC_HaltA(); // Stop reading current card
}
