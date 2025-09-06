#include <WiFi.h>
#include "config.h"

// Inisialisasi objek
#if !DEBUG_MODE
  HardwareSerial dfPlayerSerial(0); // UART0
  DFRobotDFPlayerMini myDFPlayer;
#endif

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
HardwareSerial arduinoSerial(2); // UART2 untuk komunikasi dengan Arduino

// Timing variables
unsigned long ledOnTime = 0;
String currentLedColor = "";
unsigned long soundOnTime = 0;
String currentSoundType = "";
unsigned long visualStartTime = 0;
String currentVisualType = "";
bool visualActive = false;

void debugLog(String message) {
  #if DEBUG_MODE
    Serial.println(message);
  #else
    // Dalam mode running, bisa mengirim log via WebSocket jika diperlukan
    webSocket.broadcastTXT("LOG: " + message);
  #endif
}

void stopAllOutputs() {
  // Matikan semua LED
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
  
  // Hentikan suara jika sedang diputar
  #if !DEBUG_MODE
    myDFPlayer.stop();
  #endif
  
  // Reset variabel
  currentLedColor = "";
  currentSoundType = "";
  currentVisualType = "";
  visualActive = false;
  
  // Kirim perintah reset ke Arduino
  arduinoSerial.println("RESET");
  
  debugLog("All outputs stopped");
}

void initializeDFPlayer() {
  #if !DEBUG_MODE
    debugLog("Initializing DFPlayer...");
    
    // Inisialisasi serial untuk DFPlayer
    dfPlayerSerial.begin(9600, SERIAL_8N1, 3, 1); // RX=3, TX=1 (UART0)
    
    // Tunggu sebentar untuk inisialisasi
    delay(1000);
    
    if (!myDFPlayer.begin(dfPlayerSerial)) {
      debugLog("DFPlayer initialization failed!");
      debugLog("1. Please check connections");
      debugLog("2. Please check SD card");
      
      // Berkedip dengan LED merah untuk indikasi error
      while(true) {
        digitalWrite(redLed, HIGH);
        delay(500);
        digitalWrite(redLed, LOW);
        delay(500);
      }
    }
    
    debugLog("DFPlayer Mini online.");
    myDFPlayer.volume(20);  // Set volume value (0~30)
  #else
    debugLog("DEBUG MODE: DFPlayer initialization skipped");
  #endif
}

void setup() {
  #if DEBUG_MODE
    Serial.begin(115200);
    debugLog("Debug mode active - Serial monitor enabled");
  #endif

  // Setup LED pins
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);

  // Inisialisasi DFPlayer
  initializeDFPlayer();

  // Setup WiFi AP
  WiFi.softAP("ESP32_Game_Server", "12345678");
  
  // Wait for AP to start
  delay(1000);
  debugLog("AP IP address: " + WiFi.softAPIP().toString());

  // Start serial communication with Arduino - UART2
  arduinoSerial.begin(9600, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);

  // WebSocket start
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // HTTP server routes
  server.on("/response", HTTP_POST, handleResponse);
  server.on("/reset", HTTP_POST, handleReset);
  server.begin();
  
  debugLog("Server started successfully");
}

void loop() {
  webSocket.loop();
  server.handleClient();
  
  // Handle serial communication with Arduino
  if (arduinoSerial.available()) {
    String message = arduinoSerial.readStringUntil('\n');
    message.trim();
    
    if (message == "TARGET_REACHED" && visualActive) {
      // Target reached but no response from client
      webSocket.broadcastTXT("RESPONSE_TIMEOUT:visual:" + currentVisualType);
      stopAllOutputs();
    }
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_TEXT) {
    String message = String((char*)payload);
    
    // Handle reset command
    if (message == "reset") {
      stopAllOutputs();
      webSocket.broadcastTXT("RESET:DONE");
      return;
    }
    
    // Handle LED commands
    if(message == "red" || message == "yellow" || message == "blue") {
      // Matikan output sebelumnya
      stopAllOutputs();
      
      // Turn on the specified LED
      digitalWrite(redLed, message == "red" ? HIGH : LOW);
      digitalWrite(yellowLed, message == "yellow" ? HIGH : LOW);
      digitalWrite(blueLed, message == "blue" ? HIGH : LOW);
      
      // Record time and color
      currentLedColor = message;
      ledOnTime = millis();
      
      // Notify all WebSocket clients
      webSocket.broadcastTXT("LED_ON:" + message);
      
      // Send start timer command to ESP32 Client 1
      sendStartTimerCommand(message, "led", client1IP);
    }
    // Handle sound commands
    else if(message == "siren" || message == "ambulance") {
      // Matikan output sebelumnya
      stopAllOutputs();
      
      #if !DEBUG_MODE
        // Play the specified sound
        if(message == "siren") {
          myDFPlayer.play(1);
        } else if(message == "ambulance") {
          myDFPlayer.play(2);
        }
      #else
        debugLog("DEBUG: Would play sound: " + message);
      #endif
      
      // Record time and sound type
      currentSoundType = message;
      soundOnTime = millis();
      
      // Notify all WebSocket clients
      webSocket.broadcastTXT("SOUND_ON:" + message);
      
      // Send start timer command to ESP32 Client 1
      sendStartTimerCommand(message, "sound", client1IP);
    }
    // Handle visual commands
    else if(message == "gauge" || message == "spectrum") {
      // Matikan output sebelumnya
      stopAllOutputs();
      
      // Record visual type and start time
      currentVisualType = message;
      visualStartTime = millis();
      visualActive = true;
      
      // Send command to Arduino
      arduinoSerial.println("START:" + message);
      
      // Notify all WebSocket clients
      webSocket.broadcastTXT("VISUAL_ON:" + message);
      
      // Send start timer command to ESP32 Client 2
      sendStartTimerCommand(message, "visual", client2IP);
    }
  }
}

void sendStartTimerCommand(String type, String category, const char* clientIP) {
  WiFiClient client;
  HTTPClient http;
  
  String startTimerURL = "http://" + String(clientIP) + "/startTimer";
  http.begin(client, startTimerURL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String postData = "type=" + type + "&category=" + category;
  int httpResponseCode = http.POST(postData);
  
  if(httpResponseCode == 200) {
    debugLog("Start timer command sent to " + String(clientIP) + " for " + category + ": " + type);
  } else {
    debugLog("Error sending start timer command to " + String(clientIP) + ". Error code: " + String(httpResponseCode));
  }
  
  http.end();
}

void handleResponse() {
  if(server.hasArg("type") && server.hasArg("response_time") && server.hasArg("category")) {
    String type = server.arg("type");
    unsigned long responseTime = server.arg("response_time").toInt();
    String category = server.arg("category");
    
    // Matikan output karena response sudah diterima
    stopAllOutputs();
    
    // Send response time to all WebSocket clients
    webSocket.broadcastTXT("RESPONSE_TIME:" + String(responseTime) + ":" + category + ":" + type);
    
    server.send(200, "text/plain", "OK");
    debugLog("Response time received: " + String(responseTime) + "ms for " + category + ": " + type);
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void handleReset() {
  stopAllOutputs();
  server.send(200, "text/plain", "Reset done");
  webSocket.broadcastTXT("RESET:DONE");
}