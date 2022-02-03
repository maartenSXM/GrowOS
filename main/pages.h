#ifndef PAGES_H_
#define PAGES_H_

#include "common.h"
#include "lcd.h"
#include "schedule.h"

void templatePage();
bool touchIsPressed(void);
void splashPage();
bool awaitingButtonRelease(Adafruit_GFX_Button *btn);
int countDeviceOnTime(const int devI);
void pollCurrentTime();
void maxTimeSlotsPage();
void newTimeslotPage();
void weeksSelectPage();
void daysSelectPage();
void schedulePage();
void settingPage();
void homePage();
void reDrawPage();
void updatePage();
void pageRouter();
void disablePageLimits();
void touchPage();

#endif