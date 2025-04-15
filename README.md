# Air Quality Monitor Using ESP32 with Captive Portal and Floating Widget

## Overview
This project implements an air quality monitoring system using an ESP32 microcontroller, equipped with MQ2, MQ7, and MQ135 sensors to detect smoke, carbon monoxide (CO), and carbon dioxide (CO2) levels, respectively. The system creates a captive portal Wi-Fi access point named "AirQualityMonitor," serving a webpage that displays the Air Quality Index (AQI) and gas readings in real-time via sensor data. Additionally, it supports an Android floating widget for continuous monitoring, with an APK download feature included. Sensor integration is complete, and the project is currently **ongoing**, with efforts focused on enhancing the widget's responsiveness and ensuring it fetches values from the API accurately.

## Features
- Real-time monitoring of air quality using MQ2 (smoke), MQ7 (CO), and MQ135 (CO2) sensors.
- Captive portal accessible via a Wi-Fi network ("AirQualityMonitor") with a dynamic webpage displaying AQI and gas levels.
- Android floating widget for continuous AQI updates (APK download feature included, with ongoing API integration improvements).
- mDNS support for easy access at `http://airquality.local`.
- Sensor data processed and displayed via API endpoint `/aqi`.

## Hardware Requirements
- ESP32 Development Board (e.g., ESP32 DevKitC)
- MQ2 Gas Sensor (smoke/LPG detection)
- MQ7 Gas Sensor (CO detection)
- MQ135 Gas Sensor (CO2 detection)
- Jumper wires and breadboard
- Power supply (USB or external 3.3V/5V source)

## Software Requirements
- Arduino IDE (with ESP32 board support)
- Libraries:
  - `ESPAsyncWebServer`
  - `DNSServer`
  - `ESPmDNS`
  - `LittleFS`
- Android device (for widget testing, optional)

## Installation

### 1. Set Up Arduino IDE
- Install the Arduino IDE (version 2.x recommended).
- Add ESP32 board support via **File > Preferences > Additional Boards Manager URLs**: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
- Go to **Tools > Board > Boards Manager**, search for "esp32" by Espressif, and install the latest version (e.g., 2.0.14).
- Install required libraries via **Sketch > Include Library > Manage Libraries**:
- Search and install `ESPAsyncWebServer`, `DNSServer`, `ESPmDNS`, and `LittleFS`.

### 2. Configure the Project
- Clone or download this repository to your local machine.
- Connect the ESP32 to your computer via USB.
- Wire the sensors to the ESP32:
- MQ2 to GPIO 34
- MQ7 to GPIO 35
- MQ135 to GPIO 32
- Ensure common ground and power connections (3.3V or 5V as per sensor specs).

### 3. Upload Code and Files
- Open `AQI_WebApp.ino` in the Arduino IDE.
- Select your ESP32 board and port under **Tools > Board** and **Tools > Port**.
- Upload the `.ino` file to the ESP32.
- Use the Arduino ESP32FS tool (or the built-in LittleFS uploader in newer IDE versions) to upload `index.html` to the ESP32’s LittleFS:
- Install the ESP32FS plugin if needed.
- Select **Tools > ESP32 Sketch Data Upload** and upload the `index.html` file.

### 4. APK for Floating Widget
- The APK for the Android floating widget is not included due to size constraints. Host it externally (e.g., GitHub Releases or a web server) and update the `apkUrl` in `index.html` with the correct download link (current placeholder: `https://https://github.com/Raas21/AQMS/AQI_WebApp/widget.apk`).
- Develop the widget using Android Studio if needed (sample code available upon request). Ongoing work includes improving API responsiveness.

## Usage
1. Power on the ESP32; it will create a Wi-Fi network named "AirQualityMonitor" with password "password123".
2. Connect to this network from your device (e.g., laptop or phone).
3. You will be redirected to `http://airquality.local`.
4. The webpage will display real-time AQI, category, and gas readings based on sensor data.
5. Click the "Install Widget" button on an Android device to download the APK (update the URL first). Note: Widget API fetching is under development.
6. Install and open the widget on Android for continuous monitoring (functionality pending full API integration).

## Code Structure
- **`AQI_WebApp.ino`**: Main Arduino sketch for ESP32, handling Wi-Fi, sensor integration, and webpage serving with API endpoint `/aqi`.
- **`index.html`**: Dynamic webpage fetching AQI and gas data from the `/aqi` API, with widget download functionality.

## Limitations
- Sensor calibration requires adjustment with actual gas sources for accurate ppm readings.
- APK serving is not implemented on ESP32 due to LittleFS size limits; requires external hosting.
- AQI calculation is a simplified approximation based on CO and smoke; full PM2.5 integration is future work.
- Widget API fetching is incomplete; ongoing work addresses responsiveness and data accuracy.

## Future Work
- Refine sensor calibration for precise gas measurements.
- Develop a fully functional Android floating widget with responsive API fetching.
- Enhance AQI calculation with additional pollutants (e.g., PM2.5, NO2).
- Add data logging and cloud integration for long-term monitoring.

---
*Last updated: April 15, 2025*
