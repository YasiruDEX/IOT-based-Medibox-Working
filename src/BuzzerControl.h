#include "Arduino.h"
// BuzzerControl.h
#ifndef BUZZER_CONTROL_H
#define BUZZER_CONTROL_H

class BuzzerControl {
public:
    #define BUZZER 5

  BuzzerControl(int pin);
  void setup();
  void ringAlarm();
};

#endif
