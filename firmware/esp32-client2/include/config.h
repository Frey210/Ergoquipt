#ifndef CONFIG_H
#define CONFIG_H

// WiFi settings
const char* ssid = "ESP32_Game_Server";
const char* password = "12345678";

// Server address
const char* serverURL = "http://192.168.4.1/response";

// Client 2 IP configuration
IPAddress local_ip(192, 168, 4, 11);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Button Pin
const int visualButton = 32;

#endif