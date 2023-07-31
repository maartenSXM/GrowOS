#ifndef RTC_H_
#define RTC_H_

#include "globals.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "driver/rtc_io.h"

void init_rtc();
void set_rtc_time(int year = 2023, int month = 6, int day = 24, int hour = 12, int minute = 30, int second = 0);
void initializeTimeZone();
int dater(int x);
int dateDiff(int year1, int mon1, int day1, int year2, int mon2, int day2);
uint16_t getDaysSinceNutrientDate();
void setNutrientDate();
void saveNutrientDateFlash();
void pollCurrentDateTime();
void pollCurrentDate();
void pollCurrentTime();

#endif