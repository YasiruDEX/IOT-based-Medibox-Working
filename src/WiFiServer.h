// WiFiServer.h
#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <WiFi.h>

class WiFiServerControl {
public:
  WiFiServerControl(const char *ssid, const char *password);
  void setup();
};

#endif
