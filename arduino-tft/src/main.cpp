#include "config.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// Variables for gauge animation
bool gaugeActive = false;
unsigned long gaugeStartTime;
int gaugeDuration;
int currentAngle = 0;
int targetAngle = 270; // 270 degrees for full gauge
String visualType = "";
unsigned long lastActivityTime = 0;
const unsigned long INACTIVITY_TIMEOUT = 10000; // 10 seconds inactivity timeout

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(3); // Adjust rotation as needed
  displayBrandingScreen(); // Show branding on startup
}

void loop() {
  // Check for serial commands from ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.startsWith("START:")) {
      visualType = command.substring(6);
      startVisualTest(visualType);
      lastActivityTime = millis();
    } 
    else if (command == "RESET") {
      resetDisplay();
      lastActivityTime = millis();
    }
  }
  
  // Update gauge if active
  if (gaugeActive) {
    updateGauge();
  }
  
  // Check for inactivity and return to branding screen
  if (!gaugeActive && millis() - lastActivityTime > INACTIVITY_TIMEOUT) {
    displayBrandingScreen();
    lastActivityTime = millis(); // Reset timer to prevent continuous redraw
  }
}

void displayBrandingScreen() {
  tft.fillScreen(ILI9341_BLACK);
  
  // Draw decorative elements
  drawDecorativeElements();
  
  // Display "Ergoquipt" text
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(3);
  tft.setCursor(50, 80);
  tft.println("ERGQUIPT");
  
  // Subtitle or tagline
  tft.setTextSize(1);
  tft.setCursor(60, 130);
  tft.println("Reaction Time Test System");
  
  // Version info
  tft.setTextSize(1);
  tft.setCursor(80, 200);
  tft.println("v2.0 - 2024");
  
  // Instruction for user
  tft.setTextSize(1);
  tft.setCursor(40, 220);
  tft.println("Waiting for visual test command...");
}

void drawDecorativeElements() {
  // Draw a border around the screen
  tft.drawRoundRect(5, 5, 310, 230, 10, ILI9341_BLUE);
  
  // Draw some decorative lines
  for (int i = 0; i < 8; i++) {
    int x = random(20, 300);
    int y = random(20, 220);
    int length = random(10, 50);
    tft.drawFastHLine(x, y, length, ILI9341_NAVY);
  }
  
  // Draw some circles in the corners
  tft.fillCircle(20, 20, 10, ILI9341_DARKGREY);
  tft.fillCircle(300, 20, 10, ILI9341_DARKGREY);
  tft.fillCircle(20, 220, 10, ILI9341_DARKGREY);
  tft.fillCircle(300, 220, 10, ILI9341_DARKGREY);
  
  // Draw a simple logo-like shape
  tft.fillTriangle(160, 40, 140, 70, 180, 70, ILI9341_MAROON);
  tft.fillRect(150, 70, 20, 30, ILI9341_MAROON);
}

void startVisualTest(String type) {
  gaugeActive = true;
  gaugeStartTime = millis();
  
  // Random duration between 3-7 seconds
  gaugeDuration = random(3000, 7000);
  
  // Reset display
  tft.fillScreen(ILI9341_BLACK);
  
  // Draw gauge background
  drawGaugeBackground(type);
}

void resetDisplay() {
  gaugeActive = false;
  displayBrandingScreen(); // Show branding instead of plain black screen
}

void drawGaugeBackground(String type) {
  // Draw gauge outline
  tft.drawArc(120, 120, 80, 85, 0, 360, ILI9341_WHITE);
  
  // Draw gauge labels
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(100, 50);
  
  if (type == "gauge") {
    tft.println("GAUGE TEST");
  } else if (type == "spectrum") {
    tft.println("SPECTRUM TEST");
  }
  
  tft.setTextSize(1);
  tft.setCursor(80, 220);
  tft.println("Press button when needle reaches target");
}

void updateGauge() {
  unsigned long elapsed = millis() - gaugeStartTime;
  float progress = min(1.0, (float)elapsed / gaugeDuration);
  int newAngle = progress * targetAngle;
  
  if (newAngle != currentAngle) {
    // Clear previous needle
    if (currentAngle > 0) {
      drawNeedle(currentAngle, ILI9341_BLACK);
    }
    
    // Draw new needle
    drawNeedle(newAngle, ILI9341_RED);
    
    // Draw target marker (green)
    drawTargetMarker();
    
    currentAngle = newAngle;
  }
  
  // Check if target reached
  if (elapsed >= gaugeDuration) {
    Serial.println("TARGET_REACHED");
    gaugeActive = false;
    
    // Show completion message briefly before returning to branding
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(80, 100);
    tft.println("TEST COMPLETE");
    delay(2000);
    displayBrandingScreen();
  }
}

void drawNeedle(int angle, uint16_t color) {
  float rad = angle * PI / 180;
  int x = 120 + 70 * cos(rad - PI/2);
  int y = 120 + 70 * sin(rad - PI/2);
  
  tft.drawLine(120, 120, x, y, color);
  
  // Draw needle center
  tft.fillCircle(120, 120, 5, color);
}

void drawTargetMarker() {
  // Draw target marker at 270 degrees (bottom)
  float rad = targetAngle * PI / 180;
  int x = 120 + 75 * cos(rad - PI/2);
  int y = 120 + 75 * sin(rad - PI/2);
  
  tft.fillTriangle(
    x, y,
    x-5, y-10,
    x+5, y-10,
    ILI9341_GREEN
  );
}