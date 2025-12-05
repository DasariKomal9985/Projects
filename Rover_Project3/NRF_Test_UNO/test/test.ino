#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN

void setup() {
  Serial.begin(9600);
  Serial.println("NRF24L01 Hardware Test\n");

  if (!radio.begin()) {
    Serial.println("❌ NRF24 NOT DETECTED");
    Serial.println("Check:");
    Serial.println(" - Wiring");
    Serial.println(" - 3.3V Power");
    Serial.println(" - Capacitor 10uF");
    while (1);
  }

  Serial.println("✅ NRF24 DETECTED!");
  Serial.println("Reading radio details...\n");

  radio.printDetails();
}

void loop() {
}
