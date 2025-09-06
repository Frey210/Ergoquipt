#ifndef CONFIG_H
#define CONFIG_H

// WiFi settings
const char* ssid = "ESP32_Game_Server";
const char* password = "12345678";

// Server address
const char* serverURL = "http://192.168.4.1/response";

// Client 1 IP configuration
IPAddress local_ip(192, 168, 4, 10);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Button Pins
const int redButton = 5;
const int yellowButton = 18;
const int blueButton = 19;
const int sirenButton = 0;
const int ambulanceButton = 4;

#endif