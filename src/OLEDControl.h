#include "Arduino.h"

// OLEDControl.h
#ifndef OLED_CONTROL_H
#define OLED_CONTROL_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDControl {
public:
    #define SCREEN_ADDRESS 0x3C
  
  OLEDControl(int width, int height, int resetPin, int address);
  void setup();
  void printLine(String message, int textSize, int row, int column);
  void printLineBlack(String message, int textSize, int row, int column);
  void fillRectangle(int x, int y, int width, int height);
  void clearDisplay();

private:
  Adafruit_SSD1306 display;
};

#endif
