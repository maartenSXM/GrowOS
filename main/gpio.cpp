#include "globals.h"
#include <stdio.h>
#include "gpio.h"
#include "arduino.h"

void setPin(int pin, int level)
{
#ifdef DEBUG
  printf("Pin: %d - %s\n", pin, level ? "HIGH" : "LOW");
#endif
  if (level)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  else
  {
    digitalWrite(pin, LOW);
  }
}

void allDevicesOff()
{
  for (int i = 1; i <= numDevices; i++)
  { // start 1 becuase fan(0) has no pin
    // setPin(devicePins[i], 0);
  }
}

void resetLCDPower()
{
  setPin(TFT_RST, false);
  delay(1);
  setPin(TFT_RST, true);
}

void readTempHumid()
{
  currentTemp = dht.readTemperature(); // Read temperature as Celcius (isFahrenheit = false)
  currentHumid = dht.readHumidity();

  printf("currentTemp = %d\n", currentTemp);
  printf("currentHumid = %d\n", currentHumid);

  if (currentTemp > maxTempDay)
    maxTempDay = currentTemp;
  else
    {
    if (currentTemp < minTempDay)
        minTempDay = currentTemp;
    }

  if (currentHumid > maxHumidDay)
    maxHumidDay = currentHumid;
  else
    {
    if (currentHumid < minHumidDay)
        minHumidDay = currentHumid;
    }
}

void resetMinMaxTempHumid()
{
  maxTempDay = currentTemp;
  minTempDay = currentTemp;
  maxHumidDay = currentHumid;
  minHumidDay = currentHumid;
}

// last elem in array: 0th is off duration, 1st is on duration, 2 is state, 3 is sinceStateChange
void setDevicePeriodic(const int devI)
{

  int offDur = devPeriodic[currentSlot][devI][0];
  int onDur = devPeriodic[currentSlot][devI][1];
  bool state = devPeriodic[currentSlot][devI][2];
  int secStateChange = devPeriodic[currentSlot][devI][3];
  int secSinceSlotStart = secondsDiffTime();
  bool stateChanged = false;

  if (state == 0 && ((secSinceSlotStart - secStateChange) >= offDur))
  {
    devPeriodic[currentSlot][devI][2] = !state;
    devPeriodic[currentSlot][devI][3] = secSinceSlotStart;
    stateChanged = true;
  }
  if (state == 1 && ((secSinceSlotStart - secStateChange) >= onDur))
  {
    devPeriodic[currentSlot][devI][2] = !state;
    devPeriodic[currentSlot][devI][3] = secSinceSlotStart;
    stateChanged = true;
  }
  // it has not been set yet
  if (secStateChange == 0)
  {
    devPeriodic[currentSlot][devI][3] = secSinceSlotStart;
    stateChanged = true;
    state = false;
  }
  /*
  if (stateChanged)
  {
    switch (devI)
    {
    case 0: // fan
      if (!state)
        // setFanPWM(fanSpeed);
        else
            // setFanPWM(0);
            break;
    case 1: // light
      // setPin(ACT_LED, !state);
      break;
    case 2: // pump
      // setPin(ACT_PUMP, !state);
      break;
    case 3: // 12v
      // setPin(ACT_5V, !state);
      break;
    case 4: // 5v
      // setPin(ACT_5V, !state);
      break;
    }
  }
  */
  return;
}

void setFanAuto()
{
  readTempHumid();
  if (currentTemp >= (targetTemp + 8))
  {
    fanSpeed = 100;
  }
  else if ((currentTemp < (targetTemp + 8)) && (currentTemp > targetTemp))
  {
    fanSpeed = map(currentTemp, targetTemp, targetTemp + 8, 50, 100); // maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
  }
  else if (currentTemp == targetTemp)
  {
    fanSpeed = 50;
  }
  else if ((currentTemp < targetTemp) && (currentTemp > (targetTemp - 4)))
  {
    fanSpeed = map(currentTemp, targetTemp - 4, targetTemp, 10, 50); // maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
  }
  else
  {
    fanSpeed = 10; // less than lower bound
  }

  fanSpeed = constrain(fanSpeed, 10, 100); // constrain between 0 and 100
  setFanPWM(fanSpeed);
}

void setDeviceAuto(const int devI)
{
#ifdef DEBUG
  printf("***** Adjusting a device to be auto ***** \n");
  printf("Setting dev %d to be auto\n", devI);
#endif

  int fillTime = 0;
  int onTime = 0;
  int slotTime; // seconds in a slot variable
  switch (devI)
  {
  case 0: // Fan
    setFanAuto();
    return;
  case 1:                             // light
    onTime = countDeviceOnTime(devI); // How long is it planed to be on for the current schedule;
    fillTime = lightQuota - onTime;   // Time needing to be filled with device on
    break;
  case 2: // pump
    setPumpAuto();
    return;
  case 3:                             // 12v
    onTime = countDeviceOnTime(devI); // How long is it planed to be on for the current schedule;
    fillTime = twelveVQuota - onTime; // Time needing to be filled with device on
    break;
  case 4:                             // 5v
    onTime = countDeviceOnTime(devI); // How long is it planed to be on for the current schedule;
    fillTime = fiveVQuota - onTime;   // Time needing to be filled with device on
    break;
    break;
  }
#ifdef DEBUG
  printf("Dev %d is on for %d\n", devI, onTime);
  printf("fillTime = %d\n", fillTime);
  delay(10);
#endif

  for (int i = 0; i < numSlots; i++)
  { // remove auto setting and set so quota is filled
    // if this slot is an auto slot for the device
    if (devIsAuto[i][devI])
    {
      if (fillTime > 0)
      {
        slotTime = secondsDiffTwoTimes(startTimeSlots[i], startTimeSlots[(i + 1) % numSlots]);
#ifdef DEBUG
	printf("slottime %d\n", slotTime);
#endif
        if (slotTime < fillTime)
        {
          devIsAuto[i][devI] = 0;
          devIsState[i][devI] = 1;
          devIsPeriodic[i][devI] = 0;
          fillTime -= slotTime;
        }
        if (slotTime >= fillTime)
        {
          devIsAuto[i][devI] = 0;
          devIsState[i][devI] = 0;
          devIsPeriodic[i][devI] = 1;
          devPeriodic[i][devI][1] = fillTime;
          devPeriodic[i][devI][0] = 214748364; // set to max off time seconds - 1-light......0th is off duration, 1st is on duration, 2nd is state, 3rd is sinceStateChange
          fillTime = 0;
        }
      }
      // no need to have device on, so lets only change this subsequent auto slot to off.
      else
      {
        devIsAuto[i][devI] = 0;
        devIsState[i][devI] = 0;
        devIsPeriodic[i][devI] = 0;
      }
    }
    // not an auto slot so shouldnt change it.
    else
    {
    }
  }
}

void setPumpAuto()
{
  // if light is on
  if (devIsState[currentSlot][1] || (devIsPeriodic[currentSlot][1] && (devPeriodic[currentSlot][1][1] >= devPeriodic[currentSlot][1][0])))
  {
    devIsAuto[currentSlot][2] = 0;
    devIsState[currentSlot][2] = 0;
    devIsState[currentSlot][2] = 1;
    devPeriodic[currentSlot][2][0] = timePumpOff;
    devPeriodic[currentSlot][2][1] = timePumpOn;
  }
  // light is not on, or not on alot
  else
  {
    devIsAuto[currentSlot][2] = 0;
    devIsState[currentSlot][2] = 0;
    devIsState[currentSlot][2] = 1;
    devPeriodic[currentSlot][2][0] = timePumpOff * 4.5;
    devPeriodic[currentSlot][2][1] = timePumpOn;
  }
}

void dryMode()
{
  // setPin(ACT_PUMP, false);
  // setPin(ACT_LED, false);
  // setPin(ACT_12V, false);
  // setPin(ACT_5V, false);
  targetTemp = 21;
  // setFanPWM(15);
}

void loadDeviceNames()
{
  if (customApplication)
  {
    for (int i = 0; i < numDevices; i++)
    {
      strcpy(deviceNames[i], technicalDeviceNames[i]);
    }
  }
  else
  {
    for (int i = 0; i < numDevices; i++)
    {
      strcpy(deviceNames[i], hydroponicDeviceNames[i]);
    }
  }
}

void getDeviceName(char *str, int devI)
{
  if (devI >= numDevices)
  {
    strcpy(str, "Not a valid device");
    return;
  }
  else
  {
    strcpy(str, deviceNames[devI]);
  }
}

void softwareReset()
{
  //  RSTC->RSTC_CR |= ((0xA5u << 24) | RSTC_CR_PROCRST);    //enter key set processor reset
}
