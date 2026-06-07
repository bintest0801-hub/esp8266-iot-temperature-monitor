# ESP8266 IoT Temperature Monitor 🌡️

A wireless temperature monitoring and control system 
using ESP8266, DHT11 sensor, LCD display, and a 
mobile app built with MIT App Inventor.

## ⚙️ Features

- **Real-time monitoring:** Temperature and humidity 
  displayed on LCD 16x2 via I2C
- **Auto control:** Relay automatically triggers fan 
  when temperature exceeds threshold (35°C)
- **Remote control:** Manual fan ON/OFF/AUTO via 
  MIT App Inventor mobile app over WiFi
- **Web server:** HTTP server on ESP8266 serves 
  sensor data and accepts commands

## 🛠️ Hardware

| Component | Details |
|---|---|
| MCU | ESP8266 NodeMCU v3 |
| Sensor | DHT11 (Temperature & Humidity) |
| Display | LCD 16x2 (I2C - 0x27) |
| Control | Relay module 5V (1 channel) |
| Fan | DC Fan 12V 80x80mm |
| Communication | WiFi IEEE 802.11 b/g/n |

## 📡 How It Works

1. ESP8266 connects to local WiFi network
2. DHT11 reads temperature every 3 seconds
3. Data displayed on LCD and served via HTTP
4. MIT App Inventor app sends commands to ESP8266
5. Relay controls fan based on auto/manual mode

## 📁 Project Structure
