#ifndef BUTTTONS_H_
#define BUTTTONS_H__H_

#include "common.h"

void updateWeeksSelectedBtnLabel();
void updateDaysSelectedBtnLabel();
void int_to_char_cat(char *dest,int i);
void loadGlobalButtons();
bool buttonIsHeld(Adafruit_GFX_Button *btn,int ms);
bool buttonIsPressed(Adafruit_GFX_Button *btn);
bool touchIsPressed(void);
bool awaitingButtonRelease(Adafruit_GFX_Button *btn);

#endif