#ifndef CONFIG_H
#define CONFIG_H

// Konfigurasi Mode
#define DEBUG_MODE 0  // Ubah ke 1 untuk mode debugging, 0 untuk mode running

// Pin Definitions
const int redLed = 19;
const int yellowLed = 18;
const int blueLed = 5;

// DFPlayer Mini - menggunakan UART0 (GPIO 1 TX, GPIO 3 RX)
#if DEBUG_MODE
  // Mode debugging - tidak menggunakan DFPlayer
#else
  #include <DFRobotDFPlayerMini.h>
  #include <SoftwareSerial.h>
  extern HardwareSerial dfPlayerSerial;
  extern DFRobotDFPlayerMini myDFPlayer;
#endif

// WebSocket dan Web Server
#include <WebServer.h>
#include <WebSocketsServer.h>
extern WebServer server;
extern WebSocketsServer webSocket;

// Client ESP32 details
const char* client1IP = "192.168.4.10";  // Static IP for client 1
const char* client2IP = "192.168.4.11";  // Static IP for client 2

// Serial communication with Arduino Uno - menggunakan UART2
#define SERIAL2_RX 16
#define SERIAL2_TX 17
extern HardwareSerial arduinoSerial;

// Function declarations
void debugLog(String message);
void stopAllOutputs();
void initializeDFPlayer();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void sendStartTimerCommand(String type, String category, const char* clientIP);
void handleResponse();
void handleReset();

#endif