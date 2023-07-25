#ifndef RTC_H_
#define RTC_H_

#include "globals.h"

int dater(int x);
int dateDiff(int year1,int mon1,int day1,int year2,int mon2,int day2);
uint16_t getDaysSinceNutrientDate();
void setNutrientDate();
void saveNutrientDateFlash();
void pollCurrentDate();
void pollCurrentTime();

#endif