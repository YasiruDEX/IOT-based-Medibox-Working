// BuzzerControl.cpp
#include "BuzzerControl.h"

BuzzerControl::BuzzerControl(int pin, OLEDControl &oled) 
: oledControl(oled){
  notes = new int[8]{note_A, note_B, note_C, note_D, note_F, note_E, note_G, note_C_H};
}

void BuzzerControl::setup() {
  pinMode(BUZZER, OUTPUT);
}

void BuzzerControl::ringAlarm() {
  // Implement the alarm sound logic
  oledControl.clearDisplay();
  oledControl.printLine("medicine", 2, 10, 34);
  oledControl.printLine("time", 2, 40, 51);

  digitalWrite(LED_1, HIGH);

  bool break_happened = false;

  while (digitalRead(CANCEL) == HIGH) {
    for (int i = 0; i < num_notes; i++) {
      if (digitalRead(CANCEL) == LOW) {
        delay(200);
        break_happened = true;
        break;
      }
      tone(BUZZER, notes[i]);
      delay(500);
      noTone(BUZZER);
      delay(2);
    }
  }

  delay(200);
  digitalWrite(LED_1, LOW);
}
