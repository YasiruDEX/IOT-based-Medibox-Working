// OLEDControl.cpp
#include "OLEDControl.h"

OLEDControl::OLEDControl(Adafruit_SSD1306 &display_obj)
    : display(display_obj) {
  // Additional setup if needed
}

void OLEDControl::setup() {
  Serial.begin(9600);  // Initialize Serial for debugging

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("ERROR: Display initialization failed"));
    for (;;)
      ;
  }else{
    Serial.println(F("Display initialized"));
  }

  display.display();
  delay(2000);

  display.clearDisplay();
  printLine("Welcome to medibox!", 0, 0, 2);
  delay(3000);
}


void OLEDControl::printLine(String message, int textSize, int row, int column) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(message);
}

void OLEDControl::printLineBlack(String message, int textSize, int row, int column) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(column, row);
  display.println(message);
}

void OLEDControl::clearDisplay() {
  display.clearDisplay();
}