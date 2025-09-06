#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "config.h"

// HTTP Server for receiving start commands
WebServer clientServer(80);

// Timing variables
unsigned long startTime = 0;
String currentType = "";
String currentCategory = "";

void handleStartTimer();
void handleButtonPress(String type, String category);
void sendResponseToServer(String type, unsigned long responseTime, String category);

void setup() {
  Serial.begin(115200);
  
  // Setup button pins with internal pull-up
  pinMode(redButton, INPUT_PULLUP);
  pinMode(yellowButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(sirenButton, INPUT_PULLUP);
  pinMode(ambulanceButton, INPUT_PULLUP);

  // Connect to WiFi with static IP
  WiFi.begin(ssid, password);
  WiFi.config(local_ip, gateway, subnet);
  
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("Client 1 IP: " + WiFi.localIP().toString());

  // Setup HTTP server routes
  clientServer.on("/startTimer", HTTP_POST, handleStartTimer);
  clientServer.begin();
  
  Serial.println("HTTP server started on client 1");
}

void loop() {
  clientServer.handleClient();
  
  // Check buttons only if timer has started
  if (startTime > 0) {
    if(digitalRead(redButton) == LOW) {
      handleButtonPress("red", "led");
      delay(300);
    }
    if(digitalRead(yellowButton) == LOW) {
      handleButtonPress("yellow", "led");
      delay(300);
    }
    if(digitalRead(blueButton) == LOW) {
      handleButtonPress("blue", "led");
      delay(300);
    }
    if(digitalRead(sirenButton) == LOW) {
      handleButtonPress("siren", "sound");
      delay(300);
    }
    if(digitalRead(ambulanceButton) == LOW) {
      handleButtonPress("ambulance", "sound");
      delay(300);
    }
  }
}

void handleStartTimer() {
  Serial.println("Received request to /startTimer");
  if(clientServer.hasArg("type") && clientServer.hasArg("category")) {
    currentType = clientServer.arg("type");
    currentCategory = clientServer.arg("category");
    startTime = millis();
    clientServer.send(200, "text/plain", "Timer started for " + currentCategory + ": " + currentType);
    Serial.println("Timer started for " + currentCategory + ": " + currentType);
  } else {
    clientServer.send(400, "text/plain", "Type or category parameter missing");
    Serial.println("Type or category parameter missing in request");
  }
}

void handleButtonPress(String type, String category) {
  if (type == currentType && category == currentCategory && startTime > 0) {
    unsigned long responseTime = millis() - startTime;
    sendResponseToServer(type, responseTime, category);
    startTime = 0; // Reset timer
    currentType = "";
    currentCategory = "";
  }
}

void sendResponseToServer(String type, unsigned long responseTime, String category) {
  if(WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postData = "type=" + type + "&response_time=" + String(responseTime) + "&category=" + category;
    int httpResponseCode = http.POST(postData);
    
    if(httpResponseCode == 200) {
      Serial.println("Response sent: " + category + ":" + type + " - " + String(responseTime) + "ms");
    } else {
      Serial.println("Error sending response. Error code: " + String(httpResponseCode));
      if (httpResponseCode < 0) {
        Serial.println("Error: " + http.errorToString(httpResponseCode));
      }
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}