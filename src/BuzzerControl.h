#include "Arduino.h"
// BuzzerControl.h
#ifndef BUZZER_CONTROL_H
#define BUZZER_CONTROL_H

#include "OLEDControl.h"

class BuzzerControl {
public:
  #define BUZZER 5
  #define LED_1 15

  BuzzerControl(int pin, OLEDControl &oled);
  void setup();
  void ringAlarm();

private:
  OLEDControl &oledControl;
  
  ///////////////////////////////////////// Note Definitions //////////////////////////////////////
  int num_notes = 8;
  int note_A = 220;
  int note_B = 294;
  int note_C = 330;
  int note_D = 349;
  int note_E = 440;
  int note_F = 494;
  int note_G = 450;
  int note_C_H = 523;

  char tempAr[6];
  int* notes;


};

#endif
