#ifndef GPIO_H_
#define GPIO_H_

#include "globals.h"
#include "schedule.h"

void softwareReset();
void getDeviceName(char *str,int devI);
void loadDeviceNames();
void dryMode(); 
void setPumpAuto();
int countDeviceOnTime(const int devI);
void setDeviceAuto(const int devI);
void setFanAuto();
void setFanPWM(int percent);
void setDevicePeriodic(const int devI);
void resetMinMaxTempHumid();
void readTempHumid();
void resetLCDPower();
void allDevicesOff();
void setPin(int pin,int level);

#endif