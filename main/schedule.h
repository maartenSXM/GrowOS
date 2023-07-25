#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include "globals.h"

_Time timeAdjustHours(struct _Time timeAdd, int hours);
_Time timeAdjustMinutes(_Time timeAdd, int minutes);
struct _Time timeAdjustSeconds(_Time timeAdd, int seconds);
int secondsDiffTime(_Time sT = startTimeSlots[currentSlot]);
int secondsSinceSunset(_Time cT = currentTime);
int secondsSinceSunrise(_Time cT = currentTime);
int secondsSinceNewDay(_Time cT = currentTime);
int secondsDiffTwoTimesNewDay(_Time sT, _Time sT2);
int secondsDiffTwoTimes(_Time sT = startTimeSlots[currentSlot], _Time sT2 = startTimeSlots[currentSlot]);
void removeTimeSlot(_Time tS);
void insertTimeSlot(_Time tS);
bool timeSlotExists(_Time tS);
bool isBefore(_Time t1, _Time t2);
bool isAfter(_Time t1, _Time t2);
bool isSameTime(_Time t1, _Time t2);
void setDevicePeriodic(const int devI);
void setDeviceAuto(const int devI);
void setPin(int pin, int level);
void setDeviceState();
void startNewSlot();
int8_t findCurrentSlot();
void updateDeviceStates();
bool withinCurrentSlot(int8_t slot = currentSlot);
void readTempHumid();
void pollCurrentTime();
void pollCurrentDate();
void checkSlot();
int countDeviceOnTime(const int devI);
void debugSchedule();
void saveSlotTimes();
void calcDaylightNight();
void setFanPWM(int percent);
void setNutrientDate();
void dryModeSchedule();
void defaultDailySchedule();

#endif