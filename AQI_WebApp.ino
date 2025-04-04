#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <stdlib.h>  // Required for random()

// WiFi credentials
const char* ssid = "AQM";
const char* password = "password123";

// Sensor pins (updated for ASC CSE IoT 2.0 Kit)
#define MQ2_PIN 34
#define MQ7_PIN 35
#define MQ135_PIN 27

// Calibration constants (for future use)
// #define MQ2_R0 9.83
// #define MQ7_R0 27.5
// #define MQ135_R0 3.6

AsyncWebServer server(80);
DNSServer dnsServer;

// Air quality data
float aqi = 0;
float co_ppm = 0;
float co2_ppm = 0;
float smoke_ppm = 0;

void setup() {
  Serial.begin(115200);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Start WiFi AP
  WiFi.softAP(ssid, password);
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  // Start DNS server
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Captive portal redirects
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://airquality.local");
  });
  server.on("/connectivitycheck.gstatic.com", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://airquality.local");
  });
  server.on("/captive.apple.com", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->redirect("http://airquality.local");
  });

  // Serve files
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // Data API
  server.on("/api/aqi", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "{\"aqi\":" + String(aqi) + ",\"co_ppm\":" + String(co_ppm) + ",\"co2_ppm\":" + String(co2_ppm) + ",\"smoke_ppm\":" + String(smoke_ppm) + "}";
    request->send(200, "application/json", json);
  });

  // Fallback
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("http://airquality.local");
  });

  server.begin();
  Serial.println("Server started at http://airquality.local");

  // Seed random number generator
  randomSeed(analogRead(0));
}

void loop() {
  dnsServer.processNextRequest();

  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 5000) {
    lastUpdate = millis();
    calculateAirQuality();
    Serial.println("Simulated AQI: " + String(aqi) + ", CO: " + String(co_ppm) + " ppm, CO2: " + String(co2_ppm) + " ppm, Smoke: " + String(smoke_ppm) + " ppm");
  }
}

void calculateAirQuality() {
  // Commenting out real sensor-based AQI calculation
  /*
  int mq2Value = analogRead(MQ2_PIN);
  int mq7Value = analogRead(MQ7_PIN);
  int mq135Value = analogRead(MQ135_PIN);
  // Sensor calculations...
  */

  // Simulate values
  smoke_ppm = random(100, 1000) / 10.0;  // 10.0 to 100.0 ppm
  co_ppm = random(5, 120) / 1.0;         // 5 to 120 ppm
  co2_ppm = random(350, 2000) / 1.0;     // 350 to 2000 ppm
  aqi = random(0, 300);                  // 0 to 300 AQI
}
