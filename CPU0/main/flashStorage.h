#ifndef FLASHSTORAGE_H_
#define FLASHSTORAGE_H_

#include "globals.h"

void startNewSlot();
int8_t findCurrentSlot();
void saveSlotTimes();
void loadFlashSettings();
void saveNutrientDateFlash();
void saveFlashSettings();

#endif