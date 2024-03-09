// OLEDControl.cpp
#include "OLEDControl.h"

OLEDControl::OLEDControl(int width, int height, int resetPin, int address)
    : display(width, height, &Wire, resetPin) {
  // Additional setup if needed
}

void OLEDControl::setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("ERROR"));
    for (;;)
      ;
  }

  display.display();
  delay(2000);

  display.clearDisplay();
}


void OLEDControl::printLine(String message, int textSize, int row, int column) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(column, row);
  display.println(message);
  display.display();
}

void OLEDControl::printLineBlack(String message, int textSize, int row, int column) {
  display.setTextSize(textSize);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(column, row);
  display.println(message);
  display.display();
}

void OLEDControl::fillRectangle(int x, int y, int width, int height) {
  display.fillRect(x, y, width, height, SSD1306_WHITE);
  display.display();
}


void OLEDControl::clearDisplay() {
  display.clearDisplay();
}