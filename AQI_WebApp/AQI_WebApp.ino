#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <LittleFS.h>

// Sensor pins
const int MQ2_PIN = 34;   // Analog pin for smoke/LPG detection
const int MQ7_PIN = 35;   // Analog pin for CO detection
const int MQ135_PIN = 32; // Analog pin for CO2 detection

// Wi-Fi and server setup
const char* ssid = "AirQualityMonitor";
const char* password = "password123";

AsyncWebServer server(80);
DNSServer dnsServer;

// Calibration constants (approximate Ro values in clean air)
const float MQ2_Ro = 10.0;  // kΩ
const float MQ7_Ro = 15.0;  // kΩ
const float MQ135_Ro = 20.0; // kΩ
const float RL = 10.0;      // Load resistor (kΩ)

float readMQSensor(int pin, float Ro) {
  int sensorValue = analogRead(pin);
  float VRL = (sensorValue / 4095.0) * 5.0; // ESP32 ADC range 0-4095, 0-5V
  float Rs = (5.0 / VRL - 1) * RL;          // Rs calculation
  float ratio = Rs / Ro;
  return ratio;
}

float calculateAQI(float co_ppm, float smoke_ppm) {
  // CO AQI (8-hour average) based on U.S. EPA breakpoints
  if (co_ppm <= 4.4) return (co_ppm / 4.4) * 50;
  else if (co_ppm <= 9.4) return 51 + ((co_ppm - 4.5) / 4.9) * 49;
  else if (co_ppm <= 12.4) return 101 + ((co_ppm - 9.5) / 2.9) * 49;
  else if (co_ppm <= 15.4) return 151 + ((co_ppm - 12.5) / 2.9) * 49;
  else if (co_ppm <= 30.4) return 201 + ((co_ppm - 15.5) / 14.9) * 49;
  else return 301 + ((co_ppm - 30.5) / 14.9) * 199;

  // Smoke approximated as PM2.5 (simplified, refine with actual PM sensor)
  if (smoke_ppm <= 35.4) return (smoke_ppm / 35.4) * 100;
  else if (smoke_ppm <= 55.4) return 101 + ((smoke_ppm - 35.5) / 19.9) * 49;
  else return 151; // Placeholder, adjust with full PM2.5 breakpoints
}

String getAQICategory(int aqi) {
  if (aqi <= 50) return "Good";
  else if (aqi <= 100) return "Moderate";
  else if (aqi <= 150) return "Unhealthy for Sensitive Groups";
  else if (aqi <= 200) return "Unhealthy";
  else if (aqi <= 300) return "Very Unhealthy";
  else return "Hazardous";
}

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  WiFi.softAP(ssid, password);
  Serial.println("Captive portal started...");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  if (!MDNS.begin("airquality")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started. Access at http://airquality.local");
  }

  dnsServer.start(53, "*", WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/aqi", HTTP_GET, [](AsyncWebServerRequest *request) {
    float mq2_ratio = readMQSensor(MQ2_PIN, MQ2_Ro);
    float mq7_ratio = readMQSensor(MQ7_PIN, MQ7_Ro);
    float mq135_ratio = readMQSensor(MQ135_PIN, MQ135_Ro);

    // Approximate ppm based on sensitivity curves (adjust with datasheet)
    float co_ppm = 20.0 / mq7_ratio;    // MQ7 CO calibration
    float smoke_ppm = 50.0 / mq2_ratio; // MQ2 smoke calibration
    float co2_ppm = 400.0 / mq135_ratio; // MQ135 CO2 calibration

    int aqi = calculateAQI(co_ppm, smoke_ppm);
    String category = getAQICategory(aqi);
    String response = "AQI: " + String(aqi) + " - " + category +
                     " | CO: " + String(co_ppm) + "ppm | Smoke: " + String(smoke_ppm) + "ppm | CO2: " + String(co2_ppm) + "ppm";
    request->send(200, "text/plain", response);
  });

  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");
  });
  server.on("/connectivitycheck.gstatic.com", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");
  });
  server.on("/captive.apple.com", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("/");
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  delay(1000); // Allow sensor stabilization
}
