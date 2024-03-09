 Medibox.h

#ifndef MEDIBOX_H
#define MEDIBOX_H

#include Wire.h
#include Adafruit_GFX.h
#include Adafruit_SSD1306.h
#include WiFi.h
#include DHTesp.h

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define NTP_SERVER pool.ntp.org
#define UTC_OFFSET 0
#define UTC_OFFSET_DST 0

#define CANCEL 34
#define UP 33
#define OK 32
#define DOWN 35
#define DHT_PIN 12

extern int days;
extern int month;
extern int year;
extern int hours;
extern int minutes;
extern int seconds;
extern unsigned long time_now;
extern unsigned long time_last;
extern bool alarm_enabled;
extern int num_alarms;
extern int alarm_hours[];
extern int alarm_minutes[];
extern bool alarm_triggered[];
extern int max_temp;
extern int min_temp;
extern int max_humidity;
extern int min_humidity;
extern int current_mode;
extern int prev_mode;
extern int max_modes;
extern String options[];
extern int utc_offset;
extern int prev_minutes;
extern bool showTime;
extern bool showMenu;

#endif  MEDIBOX_H
