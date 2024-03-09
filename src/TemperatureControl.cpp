// TemperatureControl.cpp
#include "TemperatureControl.h"

TemperatureControl::TemperatureControl(int dhtPin, OLEDControl &oled)
    : oledControl(oled) {
  dhtSensor.setup(dhtPin, DHTesp::DHT22);
  minTemp = 26;
  maxTemp = 32;
  minHumidity = 60;
  maxHumidity = 80;
  ledPin = LED_2;
}

void TemperatureControl::setup() {
  // Additional setup if needed
}

bool TemperatureControl::checkTemperature() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool allGood = true;

  if (data.temperature > maxTemp || data.temperature < minTemp || data.humidity > maxHumidity || data.humidity < minHumidity) {
    allGood = false;
    digitalWrite(ledPin, HIGH);
    
    // Clear the display
    oledControl.clearDisplay();

    // Display temperature and humidity messages with better formatting
    if (data.temperature > maxTemp) {
      oledControl.printLine("High Temp!", 2, 10, 0);
    } else if (data.temperature < minTemp) {
      oledControl.printLine("Low Temp!", 2, 10, 0);
    }

    if (data.humidity > maxHumidity) {
      oledControl.printLine("High Humidity!", 1, 20, 0);
    } else if (data.humidity < minHumidity) {
      oledControl.printLine("Low Humidity!", 1, 20, 0);
    }
  }

  if (allGood) {
    digitalWrite(ledPin, LOW);
  } else {
    return true;
  }

  return false;
}

