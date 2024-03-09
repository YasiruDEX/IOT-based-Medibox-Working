// TemperatureControl.h
#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#include <DHTesp.h>
#include "OLEDControl.h"

class TemperatureControl {
public:
    #define LED_2 2
  
  TemperatureControl(int dhtPin, OLEDControl &oled);
  void setup();
  bool checkTemperature();

private:
  DHTesp dhtSensor;
  OLEDControl &oledControl;  // Reference to the OLEDControl instance
  int minTemp;
  int maxTemp;
  int minHumidity;
  int maxHumidity;
  int ledPin;
};

#endif
