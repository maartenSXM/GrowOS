#ifndef TIMERFANCONTROL_H_
#define TIMERFANCONTROL_H_

#include "common.h"

void setFanPWM(int percent);
void toggleHS_AUX();
void setPin(int pin,int level);
void pwmHS_AUX();

#endif