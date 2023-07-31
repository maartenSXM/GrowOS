#ifndef BUTTTONS_H_
#define BUTTTONS_H_

#include "globals.h"

void updateWeeksSelectedBtnLabel();
void updateDaysSelectedBtnLabel();
void int_to_char_cat(char *dest, int i);
void loadGlobalButtons();
bool buttonIsHeld(ButtonWidget *btn, int ms);
bool buttonIsPressed(ButtonWidget *btn);
bool touchIsPressed(void);
bool awaitingButtonRelease(ButtonWidget *btn);

#endif