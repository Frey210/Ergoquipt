#!/bin/bash

# Script untuk deploy semua firmware ke perangkat
echo "Deploying Reaction Time Test System..."

echo "Uploading ESP32 Server..."
cd firmware/esp32-server
pio run -t upload
cd ../..

echo "Uploading ESP32 Client 1..."
cd firmware/esp32-client1
pio run -t upload
cd ../..

echo "Uploading ESP32 Client 2..."
cd firmware/esp32-client2
pio run -t upload
cd ../..

echo "Uploading Arduino TFT..."
cd arduino-tft
pio run -t upload
cd ..

echo "Deployment completed!"
echo "Please connect to WiFi: ESP32_Game_Server (password: 12345678)"
echo "Then open: http://192.168.4.1"