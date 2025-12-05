#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <DHT.h>

// TFT pins
#define TFT_CS 53
#define TFT_RST 8
#define TFT_DC 7
#define TFT_BL 9

// DHT sensor
#define DHTPIN 35
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Colors
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define GRAY 0x8410

// DHT display variables
float lastTemperature = -999;
float lastHumidity = -999;
bool firstDraw = true;
float dropletWave = 0;

// Compass variables
int compass_center_x = 200;
int compass_center_y = 120; // moved up
int compass_radius = 60;
float prev_needle_angle = -999;
float displayed_needle_angle = -999;

// Compass heading box and label
void drawCompassHeadingBox() {
  tft.fillRect(compass_center_x - 80, compass_center_y - compass_radius - 50, 160, 35, WHITE);
  tft.drawRect(compass_center_x - 80, compass_center_y - compass_radius - 50, 160, 35, BLUE);
  tft.setTextColor(BLUE);
  tft.setTextSize(3);
  tft.setCursor(compass_center_x - 60, compass_center_y - compass_radius - 45);
  tft.print("Compass");
  // Print "Compass" under the box
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(compass_center_x - 40, compass_center_y - compass_radius - 10);
  tft.print("Compass");
}

// Draw compass base (with blue line)
void drawCompassBase() {
  // Outer circle
  tft.drawCircle(compass_center_x, compass_center_y, compass_radius, WHITE);
  tft.drawCircle(compass_center_x, compass_center_y, compass_radius - 1, WHITE);

  // Inner circle
  tft.drawCircle(compass_center_x, compass_center_y, compass_radius - 20, CYAN);

  // Decorative blue line (horizontal)
  tft.drawLine(compass_center_x - compass_radius, compass_center_y, compass_center_x + compass_radius, compass_center_y, BLUE);

  // Decorative dots
  for (int i = 0; i < 360; i += 15) {
    float rad = i * PI / 180.0;
    int dot_x = compass_center_x + (compass_radius - 5) * cos(rad);
    int dot_y = compass_center_y + (compass_radius - 5) * sin(rad);
    tft.fillCircle(dot_x, dot_y, 2, YELLOW);
  }

  // Long compass points (N, S, E, W)
  for (int i = 0; i < 360; i += 90) {
    float rad = i * PI / 180.0;
    int x1 = compass_center_x + (compass_radius - 20) * cos(rad);
    int y1 = compass_center_y + (compass_radius - 20) * sin(rad);
    int x2 = compass_center_x + (compass_radius) * cos(rad);
    int y2 = compass_center_y + (compass_radius) * sin(rad);
    tft.drawLine(x1, y1, x2, y2, WHITE);
  }

  // Short compass points (NE, SE, SW, NW)
  for (int i = 45; i < 360; i += 90) {
    float rad = i * PI / 180.0;
    int x1 = compass_center_x + (compass_radius - 15) * cos(rad);
    int y1 = compass_center_y + (compass_radius - 15) * sin(rad);
    int x2 = compass_center_x + (compass_radius - 5) * cos(rad);
    int y2 = compass_center_y + (compass_radius - 5) * sin(rad);
    tft.drawLine(x1, y1, x2, y2, CYAN);
  }

  // Direction labels
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(compass_center_x - 10, compass_center_y - compass_radius - 20);
  tft.print("N");
  tft.setCursor(compass_center_x - 10, compass_center_y + compass_radius + 5);
  tft.print("S");
  tft.setCursor(compass_center_x + compass_radius + 5, compass_center_y - 10);
  tft.print("E");
  tft.setCursor(compass_center_x - compass_radius - 20, compass_center_y - 10);
  tft.print("W");

  // Diagonal labels
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(compass_center_x + compass_radius / 1.4, compass_center_y - compass_radius / 1.4);
  tft.print("NE");
  tft.setCursor(compass_center_x + compass_radius / 1.4, compass_center_y + compass_radius / 1.4);
  tft.print("SE");
  tft.setCursor(compass_center_x - compass_radius / 1.4 - 15, compass_center_y + compass_radius / 1.4);
  tft.print("SW");
  tft.setCursor(compass_center_x - compass_radius / 1.4 - 15, compass_center_y - compass_radius / 1.4);
  tft.print("NW");

  // Center circle
  tft.fillCircle(compass_center_x, compass_center_y, 8, YELLOW);
  tft.drawCircle(compass_center_x, compass_center_y, 8, WHITE);
}

// Draw compass needle
void drawCompassNeedle(float heading) {
  float angle_rad = (heading - 90) * PI / 180.0;

  // Redraw base to fix blue line issue
  drawCompassBase();

  // Calculate needle endpoints
  int needle_end_x = compass_center_x + (compass_radius - 10) * cos(angle_rad);
  int needle_end_y = compass_center_y + (compass_radius - 10) * sin(angle_rad);
  int needle_tail_x = compass_center_x - 15 * cos(angle_rad);
  int needle_tail_y = compass_center_y - 15 * sin(angle_rad);

  // Draw new needle
  for (int i = -1; i <= 1; i++) {
    tft.drawLine(compass_center_x + i, compass_center_y, needle_end_x + i, needle_end_y, RED);
  }
  tft.drawLine(compass_center_x, compass_center_y, needle_tail_x, needle_tail_y, WHITE);

  // Redraw center dot
  tft.fillCircle(compass_center_x, compass_center_y, 8, YELLOW);
  tft.drawCircle(compass_center_x, compass_center_y, 8, WHITE);
}

// DHT display functions (unchanged)
void drawTemperatureText(float temperature) {
  static float lastDrawnTemp = -999;
  if (abs(temperature - lastDrawnTemp) < 0.1) return;
  tft.fillRect(120, 80, 100, 20, BLACK);
  tft.setCursor(120, 85);
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.print("Temp: ");
  tft.setTextColor(WHITE);
  tft.print(temperature, 1);
  tft.write(247);
  tft.print("C");
  lastDrawnTemp = temperature;
}

void drawHumidityText(float humidity) {
  static float lastDrawnHum = -999;
  if (abs(humidity - lastDrawnHum) < 1.0) return;
  tft.fillRect(120, 180, 100, 20, BLACK);
  tft.setCursor(120, 185);
  tft.setTextColor(GREEN);
  tft.setTextSize(2);
  tft.print("Humi: ");
  tft.setTextColor(WHITE);
  tft.print(humidity, 1);
  tft.print(" %");
  lastDrawnHum = humidity;
}

void drawAnimatedThermometer(int x, int y, int height, float temp) {
  int level = map((int)temp, 0, 50, 0, height - 20);
  tft.fillRect(x - 20, y - height - 20, 50, height + 40, BLACK);
  int tubeWidth = 14;
  int tubeHeight = height - 15;
  tft.fillRoundRect(x - tubeWidth / 2, y - tubeHeight, tubeWidth, tubeHeight, 6, 0xE71C);
  tft.drawRoundRect(x - tubeWidth / 2, y - tubeHeight, tubeWidth, tubeHeight, 6, GRAY);
  if (level > 0) {
    tft.fillRoundRect(x - 5, y - level, 10, level, 3, RED);
  }
  tft.fillCircle(x, y + 8, 12, RED);
  tft.drawCircle(x, y + 8, 12, 0x8000);
}

void drawAnimatedHumidityDrop(int x, int y, int height, float hum) {
  int level = map((int)hum, 0, 100, 0, height - 10);
  tft.fillRect(x - 30, y - height - 10, 60, height + 25, BLACK);
  drawTeardropOutline(x, y, 22, BLUE);
  tft.fillCircle(x, y - 5, 18, 0x5E7F);
  if (level > 0) {
    dropletWave += 3;
    if (dropletWave >= 360) dropletWave = 0;
    int waveOffset = sin(dropletWave * PI / 180.0) * 2;
    for (int i = 0; i < level; i++) {
      int waterY = y - i;
      int radius = sqrt(324 - (waterY - y) * (waterY - y));
      if (radius > 0 && waterY <= y) {
        uint16_t waterColor = 0x001F + (i * 8);
        if (waterColor > 0x5E7F) waterColor = 0x5E7F;
        tft.drawLine(x - radius, waterY + waveOffset, x + radius, waterY + waveOffset, waterColor);
      }
    }
  }
  drawPercentageSymbol(x, y - 3);
}

void drawTeardropOutline(int x, int y, int radius, uint16_t color) {
  tft.drawCircle(x, y, radius, color);
  tft.drawCircle(x, y, radius + 1, color);
  int topHeight = 15;
  int topWidth = 10;
  for (int i = 0; i <= topWidth; i++) {
    int h = topHeight * sqrt(1 - (i * i) / float(topWidth * topWidth));
    tft.drawPixel(x + i, y - radius - h, color);
    tft.drawPixel(x - i, y - radius - h, color);
  }
  for (int i = 0; i < topHeight; i++) {
    int w = topWidth * sqrt(1 - (i * i) / float(topHeight * topHeight));
    tft.drawLine(x - w, y - radius - i, x + w, y - radius - i, 0x5E7F);
  }
}

void drawPercentageSymbol(int x, int y) {
  uint16_t symbolColor = BLUE;
  tft.fillCircle(x - 5, y - 5, 3, symbolColor);
  tft.fillCircle(x + 5, y + 5, 3, symbolColor);
  for (int i = -1; i <= 1; i++) {
    tft.drawLine(x - 8 + i, y + 8, x + 8 + i, y - 8, symbolColor);
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  tft.init(240, 320);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Draw compass heading box and base
  drawCompassHeadingBox();
  drawCompassBase();
}

void loop() {
  // DHT sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Sensor read failed!");
    tft.fillScreen(BLACK);
    tft.setCursor(20, 120);
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.print("Sensor Read Error!");
    delay(2000);
    return;
  }

  if (firstDraw) {
    tft.fillScreen(BLACK);
    drawCompassHeadingBox();
    drawCompassBase();
    tft.fillRect(10, 5, 310, 40, WHITE);
    tft.setCursor(60, 15);
    tft.setTextColor(BLACK);
    tft.setTextSize(3);
    tft.print("AM2305 Sensor");
    tft.drawRect(100, 75, 180, 40, YELLOW);
    tft.drawRect(100, 175, 180, 40, GREEN);
    drawAnimatedThermometer(40, 135, 70, temperature);
    drawAnimatedHumidityDrop(40, 210, 40, humidity);
    drawTemperatureText(temperature);
    drawHumidityText(humidity);
    lastTemperature = temperature;
    lastHumidity = humidity;
    firstDraw = false;
  }

  if (abs(temperature - lastTemperature) >= 0.1) {
    drawAnimatedThermometer(40, 135, 70, temperature);
    drawTemperatureText(temperature);
    lastTemperature = temperature;
  }

  if (abs(humidity - lastHumidity) >= 1.0) {
    drawAnimatedHumidityDrop(40, 210, 40, humidity);
    drawHumidityText(humidity);
    lastHumidity = humidity;
  }

  // Compass needle animation (simulate heading with millis for demo)
  float heading = fmod((millis() / 20) % 360, 360); // Replace with your real heading value
  if (displayed_needle_angle == -999) {
    displayed_needle_angle = heading;
  } else {
    float diff = heading - displayed_needle_angle;
    if (diff > 180) diff -= 360;
    if (diff < -180) diff += 360;
    displayed_needle_angle += diff * 0.2;
    if (displayed_needle_angle < 0) displayed_needle_angle += 360;
    if (displayed_needle_angle >= 360) displayed_needle_angle -= 360;
  }
  drawCompassNeedle(displayed_needle_angle);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(300);
}