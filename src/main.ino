/////////////////////////////////////// Libraries Import ///////////////////////////////////////

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <DHTesp.h>

#include "TemperatureControl.h"
#include "BuzzerControl.h"
#include "OLEDControl.h"
#include "WiFiServer.h"

///////////////////////////////////////// Pin Definitions ////////////////////////////////////////

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

#define LED_1 15
#define CANCEL 34
#define UP 33
#define OK 32
#define DOWN 35
#define DHT_PIN 12

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
int notes[] = {note_A, note_B, note_C, note_D, note_F, note_E, note_G, note_C_H};

char tempAr[6];

////////////////////////////////////// Date and Time Variables ///////////////////////////////////

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
int prev_mode = 5;
int max_modes = 5;
String options[] = {"Set Time", "Set Alarm 1", "Set Alarm 2", "Set Alarm 3", "Remove Alarm"};
int utc_offset = 19800;
int prev_minutes = 0;
bool showTime = true;
bool showMenu = true;

/////////////////////////////////////// Object Instantiations ////////////////////////////////////

OLEDControl oledControl(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, SCREEN_ADDRESS);
TemperatureControl temperatureControl(DHT_PIN, oledControl);
BuzzerControl buzzerControl(BUZZER);
WiFiServerControl wifiServerControl("Wokwi-GUEST", "");

/////////////////////////////////////////// Setup Function ////////////////////////////////////////

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
  oledControl.printLine("IoT-based-Medibox", 0, 0, 10);
  oledControl.printLine("WELCOME", 3, 20, 2);
  oledControl.printLine(".", 3, 60, 50);
  delay(1000);
  oledControl.printLine(".", 3, 60, 64);
  delay(1000);
  oledControl.printLine(".", 3, 60, 78);
  delay(1000);


  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

/////////////////////////////////////////// Loop Function /////////////////////////////////////////

void loop() {
  update_time_with_check_alarm();
  if (digitalRead(OK) == LOW) {
    delay(1000);
    Serial.println("MENU");
    showTime = true;
    showMenu = true;
    go_to_menu();
  }
  while( temperatureControl.checkTemperature()) showTime = true; 
  Serial.println(showTime);

  options[4] =  (if(alarm_enabled) ? "Disable Alarm" : "Enable Alarm") ;
  
}

////////////////////////////////////// Display Time on OLED /////////////////////////////////////

void print_time_now() {
  if (prev_minutes != minutes or showTime == true){
    oledControl.clearDisplay();

    String date = String(days) + "/" + String(month) + "/" + String(year);
    oledControl.printLine(date, 1, 0, 42);

    int twelve_hour = hours % 24;

    // Format hours and minutes as two digits
    String formatted_hours = (twelve_hour < 10) ? "0" + String(twelve_hour) : String(twelve_hour);
    String formatted_minutes = (minutes < 10) ? "0" + String(minutes) : String(minutes);

    String time_to_print = formatted_hours + ":" + formatted_minutes;

    if (twelve_hour >= 12) {
      time_to_print += "PM";
    } else {
      time_to_print += "AM";
    }

    oledControl.printLine(String(time_to_print), 3, 20, 0);

    oledControl.fillRectangle(44, 52, 40, 10);

    oledControl.printLineBlack("MENU", 1, 54, 52);

    showTime = false;
  }

  prev_minutes = minutes;
}

////////////////////////////////////// Update Time Over WiFi /////////////////////////////////////

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

////////////////////////////////// Update Time and Check Alarms /////////////////////////////////

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

////////////////////////////////////////// Ring Alarm ////////////////////////////////////////////

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

///////////////////////////////////////// Wait for Button Press ////////////////////////////////////

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

/////////////////////////////////////////// Go to Menu ////////////////////////////////////////////

void go_to_menu(void) {
  while (digitalRead(CANCEL) == HIGH) {
    if (prev_mode != current_mode or showMenu == true){

      //////////menu UI////////////
      oledControl.clearDisplay();
      
      oledControl.printLine("MENU", 1, 0, 50);
      oledControl.printLine(options[(current_mode + 4) % 5], 1, 10, 10);
      oledControl.fillRectangle(20, 22, 128-40, 12);
      oledControl.printLineBlack(options[(current_mode) % 5], 1, 24, 24);
      oledControl.printLine(options[(current_mode + 1) % 5], 1, 40, 10);
      oledControl.printLine(options[(current_mode + 2) % 5], 1, 50, 10);
      oledControl.printLine(options[(current_mode + 3) % 5], 1, 60, 10);

      /////////menu UX////////////
      prev_mode = current_mode;
      showMenu = false;

    }

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
      break;
    }
  }
}

/////////////////////////////////////////// Set Time /////////////////////////////////////////////

void set_time() {
  int temp_UTC = 0;
  int temp_hour = 0;

  while (true) {
    oledControl.clearDisplay();
    oledControl.printLine("set timer", 0, 0, 45);
    oledControl.printLine(((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) + ":00", 3, 30, 22);
    oledControl.fillRectangle(22, 55, 30, 5);

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
    oledControl.printLine("set timer", 0, 0, 45);
    String minute_phrase = ((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) +
                       ":" +
                       ((temp_minute >= 0 and temp_minute < 10) ? "0" : "") + String(temp_minute);
    oledControl.printLine(minute_phrase, 3, 30, 22);
    oledControl.fillRectangle(77, 55, 30, 5);

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
  oledControl.printLine("TIME", 2, 10, 45);
  oledControl.printLine("SET", 2, 40, 51);
  delay(1000);

}

/////////////////////////////////////////// Set Alarm ////////////////////////////////////////////

void set_alarm(int alarm) {
  int temp_hour = alarm_hours[alarm];

  while (true) {
    oledControl.clearDisplay();
    oledControl.printLine("set Alarm " + String(alarm), 0, 0, 41);
    oledControl.printLine(((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) + ":00", 3, 30, 22);
    oledControl.fillRectangle(22, 55, 30, 5);

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
    oledControl.printLine("set Alarm " + String(alarm), 0, 0, 41);
    String minute_phrase = ((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) +
                       ":" +
                       ((temp_minute >= 0 and temp_minute < 10) ? "0" : "") + String(temp_minute);
    oledControl.printLine(minute_phrase, 3, 30, 22);
    oledControl.fillRectangle(77, 55, 30, 5);

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
  oledControl.printLine("ALARM", 2, 10, 40);
  oledControl.printLine("SET", 2, 40, 51);
  delay(1000);
}

/////////////////////////////////////////// Run Mode /////////////////////////////////////////////

void run_mode(int mode) {
  if (mode == 0) {
    set_time();
  } else if (mode == 1 || mode == 2 || mode == 3) {
    set_alarm(mode - 1);
  } else if (mode == 4) {
    alarm_enabled = !alarm_enabled;
    oledControl.clearDisplay();
    oledControl.printLine("ALARM", 2, 10, 40);
    oledControl.printLine((if(alarm_enabled) ? "ENABLED" : "DISABLED"), 2, 40, 25);
    delay(2000);
  }
}