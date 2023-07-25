#ifndef PAGES_SCHEDULING_H_
#define PAGES_SCHEDULING_H_

#include "globals.h"
#include "pages.h"
#include "lcd.h"

bool touchIsPressed(void);
bool buttonIsHeld(Adafruit_GFX_Button *btn,int ms);
bool buttonIsPressed(Adafruit_GFX_Button *btn);
void insertTimeSlot(_Time tS);
bool timeSlotExists(_Time tS);
void drawSelectedTime();
void printTextBoxUL(int xPrint,int yPrint,uint16_t outline,uint16_t fill,uint16_t textColor,String s);
void maxTimeSlotsPage();
void reDrawPage();
//int secondsSinceNewDay(_Time cT);
void updateWeeksSelectedBtnLabel();
void updateDaysSelectedBtnLabel();
void newTimeslotPage();
void weeksSelectPage();
void daysSelectPage();
void homePage();
void disablePageLimits();
bool awaitingButtonRelease(Adafruit_GFX_Button *btn);
void schedulePage();
void getDeviceName(char *str,int devI);
int getTextHeight();
int getLongestDeviceNameLabelWidth();
void drawScheduleDevices(int slot,int x,int y,int slotWidth);

#endif