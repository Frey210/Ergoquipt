# Wiring Diagram

## ESP32 Server
- LED Merah → GPIO 19
- LED Kuning → GPIO 18
- LED Biru → GPIO 5
- DFPlayer RX → GPIO 1 (TX0)
- DFPlayer TX → GPIO 3 (RX0)
- Arduino RX → GPIO 17 (TX2)
- Arduino TX → GPIO 16 (RX2)

## ESP32 Client 1
- Button Merah → GPIO 5 (INPUT_PULLUP)
- Button Kuning → GPIO 18 (INPUT_PULLUP)
- Button Biru → GPIO 19 (INPUT_PULLUP)
- Button Sirine → GPIO 0 (INPUT_PULLUP)
- Button Ambulance → GPIO 4 (INPUT_PULLUP)

## ESP32 Client 2
- Visual Button → GPIO 32 (INPUT_PULLUP)

## Arduino TFT Shield
- TFT Shield terpasang langsung pada Arduino Uno
- Serial Communication dengan ESP32 Server
  - Arduino RX → ESP32 TX2 (GPIO 17)
  - Arduino TX → ESP32 RX2 (GPIO 16)