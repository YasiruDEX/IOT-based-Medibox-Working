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

void TemperatureControl::checkTemperature() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool allGood = true;

  if (data.temperature > maxTemp) {
    allGood = false;
    digitalWrite(ledPin, HIGH);
    oledControl.printLine("Temperature High", 1, 40, 0);
  } else if (data.temperature < minTemp) {
    allGood = false;
    digitalWrite(ledPin, HIGH);
    oledControl.printLine("Temperature Low", 1, 40, 0);
  }

  if (data.humidity > maxHumidity) {
    allGood = false;
    digitalWrite(ledPin, HIGH);
    oledControl.printLine("Humidity High", 1, 50, 0);
  } else if (data.humidity < minHumidity) {
    allGood = false;
    digitalWrite(ledPin, HIGH);
    oledControl.printLine("Humidity Low", 1, 50, 0);
  }

  if (allGood) {
    digitalWrite(ledPin, LOW);
  }
}
