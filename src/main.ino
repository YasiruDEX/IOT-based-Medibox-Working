#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <DHTesp.h>
#include <PubSubClient.h>

#include "TemperatureControl.h"
#include "BuzzerControl.h"
#include "OLEDControl.h"
#include "WiFiServer.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

#define LED_1 15
#define LED_2 2
#define CANCEL 34
#define UP 33
#define OK 32
#define DOWN 35
#define DHT_PIN 12

int num_notes = 8;
int note_A = 220;
int note_B = 294;
int note_C = 330;
int note_D = 349;
int note_E = 440;
int note_F = 494;
int note_G = 450;
int note_C_H = 523;
int notes[] = {note_A, note_B, note_C, note_D, note_F, note_E, note_G, note_C_H};

char tempAr[6];

int days = 0;
int month = 0;
int year = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;
unsigned long time_now = 0;
unsigned long time_last = 0;
bool alarm_enabled = true;
int num_alarms = 3;
int alarm_hours[] = {16, 1, 0};
int alarm_minutes[] = {38, 5, 10};
bool alarm_triggered[] = {false, false, false};
int max_temp = 32;
int min_temp = 26;
int max_humidity = 80;
int min_humidity = 60;
int current_mode = 0;
int max_modes = 5;
String options[] = {"1 - Set Time", "2 - Set Alarm 1", "3 - Set Alarm 2", "4 - Set Alarm 3", "5 - Remove Alarm"};
int utc_offset = 19800;

OLEDControl oledControl(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, SCREEN_ADDRESS);
TemperatureControl temperatureControl(DHT_PIN, oledControl);
BuzzerControl buzzerControl(BUZZER);
WiFiServerControl wifiServerControl("Wokwi-GUEST", "");

void setup() {
  Serial.begin(115200);
  wifiServerControl.setup();

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(CANCEL, INPUT);
  pinMode(UP, INPUT);
  pinMode(OK, INPUT);
  pinMode(DOWN, INPUT);

  Serial.begin(9600);

  temperatureControl.setup();
  buzzerControl.setup();
  oledControl.setup();

  oledControl.clearDisplay();
  oledControl.printLine("Welcome to medibox!", 0, 0, 2);
  delay(3000);

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  update_time_with_check_alarm();
  if (digitalRead(OK) == LOW) {
    delay(1000);
    Serial.println("MENU");
    go_to_menu();
  }

  temperatureControl.checkTemperature();
}

void print_time_now() {
  oledControl.clearDisplay();
   oledControl.printLine(String(days), 1, 0, 0);
   oledControl.printLine("/", 1, 0, 10);
   oledControl.printLine(String(month), 1, 0, 20);
   oledControl.printLine("/", 1, 0, 30);
   oledControl.printLine(String(year), 1, 0, 40);

   oledControl.printLine(String(hours), 2, 10, 0);
   oledControl.printLine(":", 2, 10, 20);
   oledControl.printLine(String(minutes), 2, 10, 30);
   oledControl.printLine(":", 2, 10, 50);
   oledControl.printLine(String(seconds), 2, 10, 60);
}

void update_time_over_wifi() {
  configTime(utc_offset, UTC_OFFSET_DST, NTP_SERVER);

  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char date_str[8];
  char month_str[8];
  char year_str[20];
  char hour_str[28];
  char min_str[8];
  char sec_str[8];
  strftime(month_str, 20, "%m %d %Y", &timeinfo);
  strftime(date_str, 20, "%d", &timeinfo);
  strftime(year_str, 20, "%Y", &timeinfo);
  strftime(sec_str, 8, "%S", &timeinfo);
  strftime(hour_str, 8, "%H", &timeinfo);
  strftime(min_str, 8, "%M", &timeinfo);

  days = atoi(date_str);
  month = atoi(month_str);
  year = atoi(year_str);
  hours = atoi(hour_str);
  minutes = atoi(min_str);
  seconds = atoi(sec_str);
}

void update_time_with_check_alarm() {
   oledControl.clearDisplay();
  update_time_over_wifi();
  print_time_now();

  if (alarm_enabled) {
    for (int i = 0; i < num_alarms; i++) {
      if (alarm_triggered[i] == false && hours == alarm_hours[i] && minutes == alarm_minutes[i]) {
        ring_alarm();
        alarm_triggered[i] = true;
      }
    }
  }
}

void ring_alarm() {
   oledControl.clearDisplay();
   oledControl.printLine("Medicine Time", 2, 0, 0);

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

int wait_for_button_press() {
  while (true) {
    if (digitalRead(UP) == LOW) {
      delay(200);
      return UP;
    } else if (digitalRead(DOWN) == LOW) {
      delay(200);
      return DOWN;
    } else if (digitalRead(CANCEL) == LOW) {
      delay(200);
      return CANCEL;
    } else if (digitalRead(OK) == LOW) {
      delay(200);
      return OK;
    }
  }
}

void go_to_menu(void) {
  while (digitalRead(CANCEL) == HIGH) {
     oledControl.clearDisplay();
     oledControl.printLine(options[current_mode], 2, 0, 0);

    int pressed = wait_for_button_press();

    if (pressed == UP) {
      current_mode += 1;
      current_mode %= max_modes;
      delay(200);
    } else if (pressed == DOWN) {
      current_mode -= 1;
      if (current_mode < 0) {
        current_mode = max_modes - 1;
      }
      delay(200);
    } else if (pressed == OK) {
      Serial.println(current_mode);
      delay(200);
      run_mode(current_mode);
    }
  }
}

void set_time() {
  int temp_UTC = 0;
  int temp_hour = 0;

  while (true) {
     oledControl.clearDisplay();
     oledControl.printLine("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == UP) {
      delay(200);
      temp_hour += 1;
      if (temp_hour > 14) {
        temp_hour = -12;
      }
    } else if (pressed == DOWN) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < -12) {
        temp_hour = 14;
      }
    } else if (pressed == OK) {
      delay(200);
      temp_UTC = temp_hour * 3600;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  int temp_minute = 0;

  while (true) {
     oledControl.clearDisplay();
     oledControl.printLine("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == UP) {
      delay(200);
      temp_minute += 15;
      temp_minute = temp_minute % 60;
    } else if (pressed == DOWN) {
      delay(200);
      temp_minute -= 15;
      if (temp_minute < 0) {
        temp_minute = 45;
      }
    } else if (pressed == OK) {
      delay(200);
      if (temp_minute < 0) {
        temp_minute = temp_minute * (-1);
      }
      temp_UTC += (temp_minute * 60);
      utc_offset = temp_UTC;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

   oledControl.clearDisplay();
   oledControl.printLine("Time is set", 0, 0, 2);
  delay(1000);
}

void set_alarm(int alarm) {
  int temp_hour = alarm_hours[alarm];

  while (true) {
     oledControl.clearDisplay();
     oledControl.printLine("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == UP) {
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;
    } else if (pressed == DOWN) {
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0) {
        temp_hour = 23;
      }
    } else if (pressed == OK) {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  int temp_minute = alarm_minutes[alarm];

  while (true) {
     oledControl.clearDisplay();
     oledControl.printLine("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == UP) {
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;
    } else if (pressed == DOWN) {
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0) {
        temp_minute = 59;
      }
    } else if (pressed == OK) {
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

   oledControl.clearDisplay();
   oledControl.printLine("Alarm is set", 0, 0, 2);
  delay(1000);
}

void run_mode(int mode) {
  if (mode == 0) {
    set_time();
  } else if (mode == 1 || mode == 2 || mode == 3) {
    set_alarm(mode - 1);
  } else if (mode == 4) {
    alarm_enabled = false;
  }
}

