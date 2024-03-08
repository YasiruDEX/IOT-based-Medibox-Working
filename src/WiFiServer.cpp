// WiFiServer.cpp
#include "WiFiServer.h"

WiFiServerControl::WiFiServerControl(const char *ssid, const char *password) {
  // Set up WiFi credentials
}

void WiFiServerControl::setup() {
  Serial.println("Connecting to ");
  Serial.println("Wokwi-GUEST");
  WiFi.begin("Wokwi-GUEST", "");

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
