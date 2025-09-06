# Pinout Reference

## ESP32 Server Pinout
| GPIO | Function         | Description           |
|------|------------------|-----------------------|
| 1    | TX0              | DFPlayer Mini RX      |
| 3    | RX0              | DFPlayer Mini TX      |
| 5    | Output           | Blue LED              |
| 16   | RX2              | Arduino TX            |
| 17   | TX2              | Arduino RX            |
| 18   | Output           | Yellow LED            |
| 19   | Output           | Red LED               |

## ESP32 Client 1 Pinout
| GPIO | Function         | Description           |
|------|------------------|-----------------------|
| 0    | INPUT_PULLUP     | Siren Button          |
| 4    | INPUT_PULLUP     | Ambulance Button      |
| 5    | INPUT_PULLUP     | Red Button            |
| 18   | INPUT_PULLUP     | Yellow Button         |
| 19   | INPUT_PULLUP     | Blue Button           |

## ESP32 Client 2 Pinout
| GPIO | Function         | Description           |
|------|------------------|-----------------------|
| 32   | INPUT_PULLUP     | Visual Button         |