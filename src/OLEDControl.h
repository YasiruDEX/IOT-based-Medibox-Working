#include "Arduino.h"

// OLEDControl.h
#ifndef OLED_CONTROL_H
#define OLED_CONTROL_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDControl {
public:
    #define SCREEN_ADDRESS 0x3C
  
  OLEDControl(Adafruit_SSD1306 &display_obj);
  void setup();
  void printLine(String message, int textSize, int row, int column);
  void printLineBlack(String message, int textSize, int row, int column);
  void clearDisplay();

private:
  Adafruit_SSD1306 &display;
};

#endif
