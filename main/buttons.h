#ifndef BUTTTONS_H_
#define BUTTTONS_H_

#include "globals.h"

void updateWeeksSelectedBtnLabel();
void updateDaysSelectedBtnLabel();
void int_to_char_cat(char *dest, int i);
void loadGlobalButtons();
bool buttonIsHeld(TFT_eSPI_Button *btn, int ms);
bool buttonIsPressed(TFT_eSPI_Button *btn);
bool touchIsPressed(void);
bool awaitingButtonRelease(TFT_eSPI_Button *btn);

#endif