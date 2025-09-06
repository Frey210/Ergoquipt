#ifndef CONFIG_H
#define CONFIG_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// TFT display pins
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

extern Adafruit_ILI9341 tft;

// Variables for gauge animation
extern bool gaugeActive;
extern unsigned long gaugeStartTime;
extern int gaugeDuration;
extern int currentAngle;
extern int targetAngle;
extern String visualType;
extern unsigned long lastActivityTime;
extern const unsigned long INACTIVITY_TIMEOUT;

// Function declarations
void displayBrandingScreen();
void drawDecorativeElements();
void startVisualTest(String type);
void resetDisplay();
void drawGaugeBackground(String type);
void updateGauge();
void drawNeedle(int angle, uint16_t color);
void drawTargetMarker();

#endif