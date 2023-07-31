#ifndef PAGES_H_
#define PAGES_H_

#include "globals.h"
#include "lcd.h"
#include "schedule.h"

bool awaitingButtonRelease(ButtonWidget *btn);
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
void deviceInfoPage();
void touchPage();
void initialWifiSetupPage(int n = -1);
void splashPage();
void templatePage();

#endif