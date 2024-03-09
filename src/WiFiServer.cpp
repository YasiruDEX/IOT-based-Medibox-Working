// WiFiServer.cpp
#include "WiFiServer.h"

WiFiServerControl::WiFiServerControl(const char *ssid, const char *password) {
  // Set up WiFi credentials
}

void WiFiServerControl::setup() {
  Serial.println("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  unsigned long startTime = millis();  // Record the start time

  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi within 10 seconds. Continuing with normal execution.");
  }
}
