/////////////////////////////////////// Libraries Import ///////////////////////////////////////

// Include necessary libraries for the project
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <DHTesp.h>

// Include custom classes for different components of the system
#include "TemperatureControl.h"
#include "BuzzerControl.h"
#include "OLEDControl.h"
#include "WiFiServer.h"

///////////////////////////////////////// Pin Definitions ////////////////////////////////////////

// Define constants for OLED screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// NTP server details for time synchronization
#define NTP_SERVER "pool.ntp.org"
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

// Button and DHT sensor pin definitions
#define CANCEL 34
#define UP 33
#define OK 32
#define DOWN 35
#define DHT_PIN 12

////////////////////////////////////// Date and Time Variables ///////////////////////////////////

// Variables to store date and time information
int days = 0;
int month = 0;
int year = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;
unsigned long time_now = 0;
unsigned long time_last = 0;

// Alarm settings
bool alarm_enabled = true;
int num_alarms = 3;
int alarm_hours[] = {16, 1, 0};
int alarm_minutes[] = {38, 5, 10};
bool alarm_triggered[] = {false, false, false};

// Temperature and humidity thresholds
int max_temp = 32;
int min_temp = 26;
int max_humidity = 80;
int min_humidity = 60;

// Mode control variables
int current_mode = 0;
int prev_mode = 5;
int max_modes = 5;
String options[] = {"Set Time", "Set Alarm 1", "Set Alarm 2", "Set Alarm 3", "Remove Alarm"};

// Timezone offset and other variables
int utc_offset = 19800;  // UTC+5:30
int prev_minutes = 0;
bool showTime = true;
bool showMenu = true;

/////////////////////////////////////// Object Instantiations ////////////////////////////////////

// Instantiate objects for different components
OLEDControl oledControl(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, SCREEN_ADDRESS);
TemperatureControl temperatureControl(DHT_PIN, oledControl);
BuzzerControl buzzerControl(BUZZER, oledControl);
WiFiServerControl wifiServerControl("Wokwi-GUEST", "");

/////////////////////////////////////////// Setup Function ////////////////////////////////////////

void setup() {
  // Begin serial communication
  Serial.begin(115200);

  // Setup WiFi server
  wifiServerControl.setup();

  // Setup button and LED pin modes
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(CANCEL, INPUT);
  pinMode(UP, INPUT);
  pinMode(OK, INPUT);
  pinMode(DOWN, INPUT);

  // Initialize components
  temperatureControl.setup();
  buzzerControl.setup();
  oledControl.setup();

  // Display startup message on OLED
  oledControl.clearDisplay();
  oledControl.printLine("IoT-based-Medibox", 0, 0, 10);
  oledControl.printLine("WELCOME", 3, 20, 2);
  oledControl.printLine(".", 3, 38, 43);
  delay(1000);
  oledControl.printLine(".", 3, 38, 57);
  delay(1000);
  oledControl.printLine(".", 3, 38, 71);
  delay(1000);

  // Configure NTP time synchronization
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

/////////////////////////////////////////// Loop Function /////////////////////////////////////////

void loop() {
  // Update time and check for alarms
  update_time_with_check_alarm();

  // Check for button presses and enter menu if OK button is pressed
  menuCheck();

  // Check temperature and display time while waiting
  while (temperatureControl.checkTemperature())
    showTime = true;

  // Update the text for the "Remove Alarm" option based on alarm_enabled
  options[4] = ((alarm_enabled) ? "Disable Alarm" : "Enable Alarm");
}

////////////////////////////////////// Display Time on OLED /////////////////////////////////////

// Function to display current time on the OLED screen
void print_time_now() {
  // Check if the minutes have changed or showTime flag is true
  if (prev_minutes != minutes or showTime == true) {
    // Clear the display
    oledControl.clearDisplay();

    // Display alarm indicator if enabled
    if (alarm_enabled)
      oledControl.printLine("A", 1, 0, 110);

    // Display date
    String date = String(days) + "/" + String(month) + "/" + String(year);
    oledControl.printLine(date, 1, 0, 42);

    // Convert hours to 12-hour format and format hours and minutes
    int twelve_hour = hours % 24;
    String formatted_hours = (twelve_hour < 10) ? "0" + String(twelve_hour) 
                              : String(twelve_hour);
    String formatted_minutes = (minutes < 10) ? "0" + String(minutes) 
                              : String(minutes);

    // Display time and AM/PM indicator
    String time_to_print = formatted_hours + ":" + formatted_minutes;
    if (twelve_hour >= 12) {
      time_to_print += "PM";
    } else {
      time_to_print += "AM";
    }

    oledControl.printLine(String(time_to_print), 3, 20, 0);

    // Display menu button and rectangle
    oledControl.fillRectangle(44, 52, 40, 10);
    oledControl.printLineBlack("MENU", 1, 54, 52);

    // Reset showTime flag
    showTime = false;
  }

  // Update previous minutes
  prev_minutes = minutes;
}

////////////////////////////////////// Update Time Over WiFi /////////////////////////////////////

// Function to update time using NTP over WiFi
void update_time_over_wifi() {
  // Configure time synchronization
  configTime(utc_offset, UTC_OFFSET_DST, NTP_SERVER);

  // Get local time
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  // Format time components into strings
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

  // Convert strings to integers and update global variables
  days = atoi(date_str);
  month = atoi(month_str);
  year = atoi(year_str);
  hours = atoi(hour_str);
  minutes = atoi(min_str);
  seconds = atoi(sec_str);
}

////////////////////////////////// Update Time and Check Alarms /////////////////////////////////

// Function to update time and check for alarm triggers
void update_time_with_check_alarm() {
  // Clear OLED display and update time over WiFi
  oledControl.clearDisplay();
  update_time_over_wifi();
  // Display current time
  print_time_now();

  // Check if alarms are enabled
  if (alarm_enabled) {
    // Check each alarm and trigger if the current time matches
    for (int i = 0; i < num_alarms; i++) {
      if (alarm_triggered[i] == false && hours == alarm_hours[i] 
          && minutes == alarm_minutes[i]) {
        // Trigger alarm and set alarm_triggered flag
        buzzerControl.ringAlarm();
        alarm_triggered[i] = true;
      }
    }
  }
}


///////////////////////////////////////// Wait for Button Press ////////////////////////////////////

// Function to wait for a button press and return the pressed button
int wait_for_button_press() {
  while (true) {
    // Check each button state and return the pressed button
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

// Function to navigate to the menu and handle menu interactions
void go_to_menu(void) {
  while (digitalRead(CANCEL) == HIGH) {
    // Check if the mode has changed or showMenu flag is true
    if (prev_mode != current_mode or showMenu == true) {
      // Display menu UI
      oledControl.clearDisplay();
      oledControl.printLine("MENU", 1, 0, 50);
      oledControl.printLine(options[(current_mode + 4) % 5], 1, 10, 10);
      oledControl.fillRectangle(20, 22, 128 - 40, 12);
      oledControl.printLineBlack(options[(current_mode) % 5], 1, 24, 24);
      oledControl.printLine(options[(current_mode + 1) % 5], 1, 40, 10);
      oledControl.printLine(options[(current_mode + 2) % 5], 1, 50, 10);
      oledControl.printLine(options[(current_mode + 3) % 5], 1, 60, 10);

      // Update previous mode and reset showMenu flag
      prev_mode = current_mode;
      showMenu = false;
    }

    // Wait for button press
    int pressed = wait_for_button_press();

    // Handle button presses
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
      // Run the selected mode and exit the loop
      run_mode(current_mode);
      break;
    }
  }
}

// Function to check if the OK button is pressed and enter the menu
void menuCheck() {
  if (digitalRead(OK) == LOW) {
    delay(1000);
    Serial.println("MENU");
    // Set flags to update time and menu display
    showTime = true;
    showMenu = true;
    // Enter the menu
    go_to_menu();
  }
}

/////////////////////////////////////////// Set Time /////////////////////////////////////////////

// Function to set the system time
void set_time() {
  int temp_UTC = 0;
  int temp_hour = 0;

  // Loop for setting the hour
  while (true) {
    // Clear the display and show the current hour setting
    oledControl.clearDisplay();
    oledControl.printLine("set timer", 0, 0, 45);
    oledControl.printLine(((temp_hour >= 0 and temp_hour < 10) ? "0" : "") 
                          + String(temp_hour) + ":00", 3, 30, 22);
    oledControl.fillRectangle(22, 55, 30, 5);

    // Wait for button press
    int pressed = wait_for_button_press();

    // Handle button presses
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
      // Calculate UTC offset based on the selected hour
      temp_UTC = temp_hour * 3600;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  // Loop for setting the minute
  int temp_minute = 0;
  while (true) {
    // Clear the display and show the current minute setting
    oledControl.clearDisplay();
    oledControl.printLine("set timer", 0, 0, 45);
    String minute_phrase = ((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) +
                           ":" +
                           ((temp_minute >= 0 and temp_minute < 10) ? "0" : "") + String(temp_minute);
    oledControl.printLine(minute_phrase, 3, 30, 22);
    oledControl.fillRectangle(77, 55, 30, 5);

    // Wait for button press
    int pressed = wait_for_button_press();

    // Handle button presses
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
      // Add minutes to UTC offset and update system's UTC offset
      temp_UTC += (temp_minute * 60);
      utc_offset = temp_UTC;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  // Show success message on OLED
  oledControl.clearDisplay();
  oledControl.printLine("TIME", 2, 10, 45);
  oledControl.printLine("SET", 2, 40, 51);
  delay(1000);
}

/////////////////////////////////////////// Set Alarm ////////////////////////////////////////////

// Function to set the alarm time
void set_alarm(int alarm) {
  int temp_hour = alarm_hours[alarm];

  // Loop for setting the hour
  while (true) {
    // Clear the display and show the current hour setting for the alarm
    oledControl.clearDisplay();
    oledControl.printLine("set Alarm " + String(alarm), 0, 0, 41);
    oledControl.printLine(((temp_hour >= 0 and temp_hour < 10) ? "0" : "") 
                            + String(temp_hour) + ":00", 3, 30, 22);
    oledControl.fillRectangle(22, 55, 30, 5);

    // Wait for button press
    int pressed = wait_for_button_press();

    // Handle button presses
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
      // Update alarm hour
      alarm_hours[alarm] = temp_hour;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  // Loop for setting the minute
  int temp_minute = alarm_minutes[alarm];
  while (true) {
    // Clear the display and show the current minute setting for the alarm
    oledControl.clearDisplay();
    oledControl.printLine("set Alarm " + String(alarm), 0, 0, 41);
    String minute_phrase = ((temp_hour >= 0 and temp_hour < 10) ? "0" : "") + String(temp_hour) +
                           ":" +
                           ((temp_minute >= 0 and temp_minute < 10) ? "0" : "") + String(temp_minute);
    oledControl.printLine(minute_phrase, 3, 30, 22);
    oledControl.fillRectangle(77, 55, 30, 5);

    // Wait for button press
    int pressed = wait_for_button_press();

    // Handle button presses
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
      // Update alarm minute
      alarm_minutes[alarm] = temp_minute;
      break;
    } else if (pressed == CANCEL) {
      delay(200);
      break;
    }
  }

  // Show success message on OLED
  oledControl.clearDisplay();
  oledControl.printLine("ALARM", 2, 10, 40);
  oledControl.printLine("SET", 2, 40, 51);
  delay(1000);
}

/////////////////////////////////////////// Run Mode /////////////////////////////////////////////

// Function to execute different modes based on the menu selection
void run_mode(int mode) {
  if (mode == 0) {
    // Set system time
    set_time();
  } else if (mode == 1 || mode == 2 || mode == 3) {
    // Set alarm time based on the menu selection
    set_alarm(mode - 1);
  } else if (mode == 4) {
    // Toggle alarm enable/disable
    alarm_enabled = !alarm_enabled;
    // Display status message on OLED
    oledControl.clearDisplay();
    oledControl.printLine("ALARM", 2, 10, 40);
    oledControl.printLine(((alarm_enabled) ? "ENABLED" : "DISABLED"), 2, 40, 25);
    delay(2000);
  }
}
