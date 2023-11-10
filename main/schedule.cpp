#include "schedule.h"

// Two slots of a time - the first slot is the human waking hours, followed by sleeping hours
// Convention should be that the first slot is the first time the light goes on for that day
void defaultDailySchedule()
{
  targetTemp = 24;

  // Set the startTimeSlots, and the devIsAuto, devIsState, devIsPeriodic for each slot and device.
  numSlots = 2;
  // first slot
  int slot = 0;
  startTimeSlots[slot] = sunriseTime;
  devIsAuto[slot][0] = 1; // Fan, light, pump, 5v,
  devIsAuto[slot][1] = 0;
  devIsAuto[slot][2] = 0;
  devIsAuto[slot][3] = 0;
  devIsState[slot][0] = 0;
  devIsState[slot][1] = 1;
  devIsState[slot][2] = 1;
  devIsState[slot][3] = 0;
  for (int i = 0; i < numDevices; i++)
  {
    devIsPeriodic[slot][i] = 0;
  }

  // secondSlot
  slot = 1;
  startTimeSlots[slot] = sunsetTime;
  devIsAuto[slot][0] = 1; // Fan on, light off, pump(on),
  devIsAuto[slot][1] = 0;
  devIsAuto[slot][2] = 0;
  devIsAuto[slot][3] = 0;
  devIsState[slot][0] = 0;
  devIsState[slot][1] = 0;
  devIsState[slot][2] = 1;
  devIsState[slot][3] = 0;
  for (int i = 0; i < numDevices; i++)
  {
    devIsPeriodic[slot][i] = 0;
  }
}

void dryModeSchedule()
{
  setNutrientDate();
  targetTemp = 20;

  // Set the startTimeSlots, and the devIsAuto, devIsState, devIsPeriodic for each slot and device.
  numSlots = 2;
  // first slot
  int slot = 0;
  startTimeSlots[slot] = sunriseTime;
  devIsAuto[slot][0] = 1; // Fan, light, pump, 5v,
  devIsAuto[slot][1] = 0;
  devIsAuto[slot][2] = 0;
  devIsAuto[slot][3] = 0;
  devIsState[slot][0] = 0;
  devIsState[slot][1] = 0;
  devIsState[slot][2] = 0;
  devIsState[slot][3] = 0;
  for (int i = 0; i < numDevices; i++)
  {
    devIsPeriodic[slot][i] = 0;
  }

  // secondSlot
  slot = 1;
  startTimeSlots[slot] = sunsetTime;
  devIsAuto[slot][0] = 1; // Fan, light, pump, 5v,
  devIsAuto[slot][1] = 0;
  devIsAuto[slot][2] = 0;
  devIsAuto[slot][3] = 0;
  devIsState[slot][0] = 0;
  devIsState[slot][1] = 0;
  devIsState[slot][2] = 0;
  devIsState[slot][3] = 0;
  for (int i = 0; i < numDevices; i++)
  {
    devIsPeriodic[slot][i] = 0;
  }

  setFanPWM(30);
}

// bringing in from globals, slot 0 is always the sunrise to sunset slot in this system
void calcDaylightNight()
{
  for (int i = 0; i < numSlots; i++)
  {
  }
  if (!(devIsState[0][1]) && !(devIsAuto[0][1]))
  { // sun not on in slot 0, maybe dry mode...
    daylight = 0;
  }
  else if ((devIsState[0][1]) || (devIsAuto[0][1]))
  { // sun is on in slot 0
    if (startTimeSlots[1].hour > startTimeSlots[0].hour)
      daylight = startTimeSlots[1].hour - startTimeSlots[0].hour; // slot 1 ends in same day
    else
      daylight = (24 - startTimeSlots[0].hour) + startTimeSlots[1].hour; // slot 1 ends in next day
  }

  night = 24 - daylight;
}

void saveSlotTimes()
{ // requires sunriseTime.hour and daylight to be set
  sunsetTime.hour = (sunriseTime.hour + daylight) % 24;
  startTimeSlots[0] = sunriseTime;
  startTimeSlots[numSlots - 1] = sunsetTime;
  if (daylight == 24)
  {
    devIsState[0][1] = 1;            // sun on in first slot
    devIsState[numSlots - 1][1] = 1; // sun also on for second of the two slots
  }
  else if (night == 24)
  {
    devIsState[0][1] = 0;            // sun off in first slot
    devIsState[numSlots - 1][1] = 0; // sun also off for second of the two slots
  }
  else
  {
    devIsState[0][1] = 1;            // typical state, on in first slot - convention is first slot is sunrise
    devIsState[numSlots - 1][1] = 0; // off for night
  }
}

// to be called on boot with valid daylight, night and sunriseTime variables.
void resetSlot()
{

  // load currentDate with current date so we can set prevDayOfWeek to be same and not instantly roll over to a new day the next time the date/time is checked.
  pollCurrentDate();
  prevDayOfWeek = currentDate.dayOfWeek;

  // SAVE
  saveSlotTimes(); // requires  sunriseTime.hour and daylight to be set

  delay(200);
  // Start new schedule
  findCurrentSlot();
  startNewSlot();
}

void debugSchedule()
{
  char charBuff[40];
  Serial.println("*****DEBUGGING SCHEDULE*****");
  sprintf(charBuff, "currentSlot = *%d*, numSlots = %d", currentSlot, numSlots);
  Serial.println(charBuff);
  for (int8_t i = 0; i < (numSlots); i++)
  {
    strcpy(charBuff, "");
    sprintf(charBuff, "Slot #%d: \n Starts at H:%d ends at H:%d", i, startTimeSlots[i].hour, startTimeSlots[(i + 1) % numSlots].hour);
    Serial.println(charBuff);
  }
  Serial.print("Light on for seconds: ");
  Serial.println(countDeviceOnTime(1));
  Serial.print("daylight: ");
  Serial.println(daylight);
  Serial.print("night: ");
  Serial.println(night);
  Serial.println(" ");
}

void checkSlot()
{
  pollCurrentDate();
  pollCurrentTime();
  // poll sensors
  readTempHumid();

  if (withinCurrentSlot())
  { // enters if in currentSlot Still - check if a device needs to be periodically altered if auto, or if set to be periodic
    updateDeviceStates();
  }
  else
  { // move to next slot...or the slot after that if we missed one...
    findCurrentSlot();
    startNewSlot();
  }
}

// initialize devices to their initial state
void startNewSlot()
{
#ifdef DEBUG
  Serial.println("Starting new slot");
#endif
  // reset secondsSinceStateChange to zero
  for (int i = 0; i < numDevices; i++)
  {
    devPeriodic[currentSlot][i][3] = 0;
  }

  // set device state manually - those that are Auto-0, and Periodic-0
  setDeviceState();

  // update periodic or auto devices
  updateDeviceStates();
}

// For state set manually devices - those that are Auto-0, and Periodic-0
void setDeviceState()
{
  /*
  if (!devIsAuto[currentSlot][0] && !devIsPeriodic[currentSlot][0] && devIsState[currentSlot][0])
    // setFanPWM(fanSpeed);
    if (!devIsAuto[currentSlot][1] && !devIsPeriodic[currentSlot][1])
      // setPin(ACT_LED, devIsState[currentSlot][1]);
      if (!devIsAuto[currentSlot][2] && !devIsPeriodic[currentSlot][2])
        // setPin(ACT_PUMP, devIsState[currentSlot][2]);
        if (!devIsAuto[currentSlot][3] && !devIsPeriodic[currentSlot][3])
  // setPin(ACT_5V, devIsState[currentSlot][3]);
  */
}

// for auto and periodically on devices, manually set devices should already be set
void updateDeviceStates()
{
  // Fan
  if (devIsAuto[currentSlot][0])
    setDeviceAuto(0);
  else if (devIsPeriodic[currentSlot][0])
    setDevicePeriodic(0);

  // Light
  if (devIsAuto[currentSlot][1])
    setDeviceAuto(1);
  else if (devIsPeriodic[currentSlot][1])
    setDevicePeriodic(1);

  // Pump
  if (devIsAuto[currentSlot][2])
    setDeviceAuto(2);
  else if (devIsPeriodic[currentSlot][2])
    setDevicePeriodic(2);

  // 12V
  if (devIsAuto[currentSlot][3])
    setDeviceAuto(3);
  else if (devIsPeriodic[currentSlot][3])
    setDevicePeriodic(3);

  // 5V
  if (devIsAuto[currentSlot][4])
    setDeviceAuto(4);
  else if (devIsPeriodic[currentSlot][4])
    setDevicePeriodic(4);
}

int8_t findCurrentSlot()
{
  for (int i = 0; i < numSlots; i++)
  {
    if (withinCurrentSlot(i))
    {
#ifdef DEBUG
      Serial.println("now in slot " + String(i));
#endif
      currentSlot = i;
      break;
    }
  }
  return currentSlot;
}

// uses currentTime Variable to compare with current slot times to see if we are or are not in the same schedule
bool withinCurrentSlot(int8_t slot)
{
  if ((slot < 0) || (slot >= numSlots))
  {
#ifdef DEBUG
    Serial.println("***********ERROR************ \n NOT A VALID SLOT NUMBER: ");
    Serial.println(slot);
#endif
    slot = slot % numSlots;
  }
  if (numSlots <= 1)
  {
    if (slot == 0)
      return true;
    else
      return false;
  }

  bool result;
  _Time SS = startTimeSlots[slot];
  _Time SE = startTimeSlots[(slot + 1) % numSlots];
#ifdef DEBUG
  Serial.println("checking if in slot #" + String(slot));
  Serial.println("Start Slot" + String(SS.hour) + ":" + String(SS.minute) + "." + String(SS.second) + "   --->    End Slot " + String(SE.hour) + ":" + String(SE.minute) + "." + String(SE.second));
#endif

  if ((numSlots == 2) && isSameTime(SS, SE))
  { // means either is all daylight (slot 0), or all night (slot 1)
    if ((daylight == 24) && (slot == 0))
      result = true;
    else if ((night == 24) && (slot == 1))
      result = true;
    else
      result = false;
  }
  else if (
      // for times where SE is numerically after SS
      (isAfter(currentTime, SS) && isBefore(currentTime, SE)) ||
      // for times where SE is numerically before SS such as when the end time passes midnight
      (isBefore(SE, SS) && isAfter(currentTime, SS)) ||
      (isBefore(SE, SS) && isBefore(currentTime, SE)))
  {
    result = true;
  }
  else
  {
    result = false;
  }
#ifdef DEBUG
  if (result)
    Serial.println("YES in slot: " + String(slot));
  else
    Serial.println("NOT in slot: " + String(slot));
#endif
  return result;
}

// logic of if t1 and t2 are equal
bool isSameTime(_Time t1, _Time t2)
{
  return ((t1.hour == t2.hour) && (t1.minute == t2.minute) && (t1.second == t2.second));
}

bool timeSlotExists(_Time tS)
{
  for (int i = 0; i < numSlots; i++)
  {
    if ((tS.hour == startTimeSlots[i].hour) && (tS.minute == startTimeSlots[i].minute))
    {
      return true;
    }
  }
  return false;
}

void insertTimeSlot(_Time tS)
{
  if (numSlots == maxNumSlots)
    return;
  if (timeSlotExists(tS))
    return; // if same hour and min as another exit

  int slot = 0;
  for (int i = 0; i < numSlots; i++)
  { // find the slot index val it will go into
    if (!isBefore(startTimeSlots[i], tS))
    {
      slot = i;
      break;
    }
    if (i == numSlots - 1)
    {
      slot = numSlots;
      break;
    }
  }

  // Shift everything from slot onwards up one in list, start from end so not to overwrite
  for (int i = numSlots; i > slot; i--)
  {
    for (int j = 0; j < numDevices; j++)
      devIsAuto[i][j] = devIsAuto[i - 1][j];
    for (int j = 0; j < numDevices; j++)
      devIsState[i][j] = devIsState[i - 1][j];
    for (int j = 0; j < numDevices; j++)
      devIsPeriodic[i][j] = devIsPeriodic[i - 1][j];
    for (int j = 0; j < numDevices; j++)
    {
      for (int k = 0; k < 4; k++)
        devPeriodic[i][j][k] = devPeriodic[i - 1][j][k];
    };
    startTimeSlots[i] = startTimeSlots[i - 1];
  }
  numSlots += 1;

  // insert the slot and get previous settings
  int fromSlot;
  if (slot == 0)
    fromSlot = slot + 1; // first slot
  else
    fromSlot = slot - 1; // middle or last slot
  for (int j = 0; j < numDevices; j++)
    devIsAuto[slot][j] = devIsAuto[fromSlot][j];
  for (int j = 0; j < numDevices; j++)
    devIsState[slot][j] = devIsState[fromSlot][j];
  for (int j = 0; j < numDevices; j++)
    devIsPeriodic[slot][j] = devIsPeriodic[fromSlot][j];
  for (int j = 0; j < numDevices; j++)
  {
    for (int k = 0; k < 4; k++)
      devPeriodic[slot][j][k] = devPeriodic[fromSlot][j][k];
  };
  startTimeSlots[slot] = tS;
}

void removeTimeSlot(_Time tS)
{
  if (numSlots == 1)
    return; // cannot have less than 1 slot
  if (!timeSlotExists(tS))
    return; // timeslot does not exist

  int slot = 0;
  for (int i = 0; i < numSlots; i++)
  { // find the slot index val to be removed
    if (timeSlotExists(startTimeSlots[i]))
    {
      slot = i;
      break;
    }
    if (i == numSlots - 1)
    {
      return; // slot does not exist
    }
  }

  _Time startT = startTimeSlots[slot];

  // Shift everything from slot onwards, down in list, start from slot so not to overwrite slots being kept
  for (int i = slot; i < (numSlots - 1); i++)
  {
    for (int j = 0; j < numDevices; j++)
      devIsAuto[i][j] = devIsAuto[i + 1][j];
    for (int j = 0; j < numDevices; j++)
      devIsState[i][j] = devIsState[i + 1][j];
    for (int j = 0; j < numDevices; j++)
      devIsPeriodic[i][j] = devIsPeriodic[i + 1][j];
    for (int j = 0; j < numDevices; j++)
    {
      for (int k = 0; k < 4; k++)
        devPeriodic[i][j][k] = devPeriodic[i + 1][j][k];
    };
    startTimeSlots[i] = startTimeSlots[i + 1];
  }
  numSlots -= 1;
}

// returns logic of if t1 is before t2. if equal.....false.....
bool isBefore(_Time t1, _Time t2)
{
  if ((t1.hour < t2.hour) ||
      ((t1.hour == t2.hour) && (t1.minute < t2.minute)) ||
      ((t1.hour == t2.hour) && (t1.minute == t2.minute) && (t1.second < t2.second)))
    return true;
  else
    return false;
}

// returns logic of if t1 is After t2. if equal.....true.....
bool isAfter(_Time t1, _Time t2)
{
  if ((t1.hour > t2.hour) ||
      ((t1.hour == t2.hour) && (t1.minute > t2.minute)) ||
      ((t1.hour == t2.hour) && (t1.minute == t2.minute) && (t1.second >= t2.second)))
    return true;
  else
    return false;
}

int countDeviceOnTime(const int devI)
{
  int seconds = 0;

  // weird case
  if ((numSlots == 2) && (devIsState[0][devI] || devIsAuto[0][devI]) && (devIsState[1][devI] || devIsAuto[1][devI]))
  {
    seconds = secondsInDay;
  }
  else
  {
    for (int slot = 0; slot < numSlots; slot++)
    {
      if (devIsState[slot][devI] && (!devIsPeriodic[slot][devI]))
      {
        if (isBefore(startTimeSlots[slot], startTimeSlots[(slot + 1) % numSlots]))
          seconds += secondsDiffTwoTimes(startTimeSlots[slot], startTimeSlots[(slot + 1) % numSlots]);
        // else a the end time is seemingly before the start because it wrapped through a new day;
        else
          seconds += secondsDiffTwoTimesNewDay(startTimeSlots[slot], startTimeSlots[(slot + 1) % numSlots]);
      }
      else if ((!devIsAuto[slot][devI]) && devIsPeriodic[slot][devI])
      {
        int secInSlot = secondsDiffTwoTimes(startTimeSlots[slot], startTimeSlots[(slot + 1) % numSlots]);
        int offDur = devPeriodic[slot][devI][0];
        int onDur = devPeriodic[slot][devI][1];
        int period = offDur + onDur;
        seconds += (onDur * (secInSlot / period));
      }
    }
  }
#ifdef DEBUG
  Serial.println("Dev " + String(devI) + " is on for " + String(seconds) + "s");
#endif
  return seconds;
}

int secondsSinceNewDay(_Time cT)
{
  if (cT.hour == 0 && cT.minute == 0 && cT.second == 0)
  {
    // If cT is not provided or is set to 00:00:00, use the current time
    cT = currentTime;
  }

  return cT.hour * 60 * 60 + cT.minute * 60 + cT.second;
}

int secondsSinceSunrise(_Time cT)
{ // if sunrise not yet, returns the time to sunrise as negative
  int sunriseTimeSeconds = 0;
  int elapsedTime = 0;
  for (int slot = 0; slot < numSlots; slot++)
  { // find the first slot the light([1]) is on, that is when sunRise is
    if (devIsAuto[slot][1] || devIsState[slot][1])
    {
      sunriseTimeSeconds = secondsSinceNewDay(startTimeSlots[slot]);
      elapsedTime = secondsSinceNewDay() - sunriseTimeSeconds;
      if (elapsedTime < 0)
      {
        elapsedTime += secondsInDay;
      }
      return elapsedTime;
    }
  }
  return 0; // no sunrise
}

int secondsSinceSunset(_Time cT)
{
  int sunsetTimeSeconds = 0;
  int elapsedTime = 0;
  for (int slot = 0; slot < numSlots; slot++)
  { // find the first slot the light([1]) is Off, that is when sunset is
    if ((!devIsAuto[slot][1]) && (!devIsState[slot][1]))
    {
      sunsetTimeSeconds = secondsSinceNewDay(startTimeSlots[slot]);
      elapsedTime = secondsSinceNewDay() - sunsetTimeSeconds;
      if (elapsedTime < 0)
      {
        elapsedTime += secondsInDay;
      }
      return elapsedTime;
    }
  }
  return elapsedTime;
}

// find the seconds difference between the current time and the since/start_Time Object
int secondsDiffTime(_Time sT)
{ // default to current time
  pollCurrentTime();
  int seconds = 0;
  int hours = 0;
  int minutes = 0;
  if ((sT.hour == currentTime.hour) && (sT.minute == currentTime.minute) && (sT.second == currentTime.second))
  {
    return seconds;
  }
  // if sT is in future - opposite to arguments so negative
  if ((currentTime.hour < sT.hour) || ((currentTime.hour == sT.hour) && (currentTime.minute < sT.minute)) || ((currentTime.hour == sT.hour) && (currentTime.minute == sT.minute) && (currentTime.second < sT.second)))
  {
    hours = sT.hour - currentTime.hour;
    minutes = sT.minute - currentTime.minute;
    if (minutes < 0)
    {
      minutes = minutes % 60; // fix for wrap around minutes
      hours -= 1;
    }
    seconds = sT.second - currentTime.second;
    if (seconds < 0)
    {
      seconds = seconds % 60; // if seconds have wrapped around
      minutes -= 1;
    }
    return -(hours * 3600 + minutes * 60 + seconds);
  }
  // sT is in past as expected so account for positive wrap for minutes and seconds
  else
  {
    hours = currentTime.hour - sT.hour;
    minutes = currentTime.minute - sT.minute;
    if (minutes < 0)
    {
      minutes = minutes % 60; // fix for wrap around minutes
      hours -= 1;
    }
    seconds = currentTime.second - sT.second;
    if (seconds < 0)
    {
      seconds = seconds % 60; // if seconds have wrapped around
      minutes -= 1;
    }
    return hours * 3600 + minutes * 60 + seconds;
  }
}

// find the seconds difference between the current time and the since/start_Time Object
int secondsDiffTwoTimes(_Time sT, _Time sT2)
{
  int seconds = 0;
  int hours = 0;
  int minutes = 0;
  if ((sT.hour == sT2.hour) && (sT.minute == sT2.minute) && (sT.second == sT2.second))
  {
    return seconds;
  }
  // if sT is in future - opposite to arguments so negative
  if ((sT2.hour < sT.hour) || ((sT2.hour == sT.hour) && (sT2.minute < sT.minute)) || ((sT2.hour == sT.hour) && (sT2.minute == sT.minute) && (sT2.second < sT.second)))
  {
    hours = sT.hour - sT2.hour;
    minutes = sT.minute - sT2.minute;
    if (minutes < 0)
    {
      minutes = minutes % 60; // fix for wrap around minutes
      hours -= 1;
    }
    seconds = sT.second - sT2.second;
    if (seconds < 0)
    {
      seconds = seconds % 60; // if seconds have wrapped around
      minutes -= 1;
    }
    return -(hours * 3600 + minutes * 60 + seconds);
  }
  // sT is in past as expected so account for positive wrap for minutes and seconds
  else
  {
    hours = sT2.hour - sT.hour;
    minutes = sT2.minute - sT.minute;
    if (minutes < 0)
    {
      minutes = minutes % 60; // fix for wrap around minutes
      hours -= 1;
    }
    seconds = sT2.second - sT.second;
    if (seconds < 0)
    {
      seconds = seconds % 60; // if seconds have wrapped around
      minutes -= 1;
    }
    return hours * 3600 + minutes * 60 + seconds;
  }
}

// find the seconds difference between the current time and the since/start_Time Object,
// when sT2 is in a new day thus hours are less than sT becuase it wrapped to a new day
int secondsDiffTwoTimesNewDay(_Time sT, _Time sT2)
{
  int seconds = 0;
  int hours = 0;
  int minutes = 0;
  if ((sT.hour == sT2.hour) && (sT.minute == sT2.minute) && (sT.second == sT2.second))
  {
    return seconds;
  }
  // sT2 "SEEMINGLY" in past but is actually in future because of new day wrap around for hours reseeting through 0
  if (sT2.hour < sT.hour)
  {
    hours = sT.hour - sT2.hour;
    minutes = sT.minute - sT2.minute;
    if (minutes < 0)
    {
      minutes = minutes % 60; // fix for wrap around minutes
      hours -= 1;
    }
    seconds = sT.second - sT2.second;
    if (seconds < 0)
    {
      seconds = seconds % 60; // if seconds have wrapped around
      minutes -= 1;
    }
    return secondsInDay - (hours * 3600 + minutes * 60 + seconds);
  }
  // Where is the new day wrap arround? Not using this function properly if it goes here
  // sT is in past as expected for a regular version of this function type
  else
  {
    return secondsDiffTwoTimes(sT, sT2);
  }
}

struct _Time timeAdjustSeconds(_Time timeAdd, int seconds)
{
  if (seconds == 0)
    return timeAdd;
  if (seconds > 0)
  { // positive seconds
    if (seconds < (60 - timeAdd.second))
    { // the seconds to be added fit in this minute
      timeAdd.second = timeAdd.second + seconds;
    }
    else
    { // the seconds dont fit
      int multiple = seconds / 60;
      int remainder = seconds % 60;
      timeAdd.second = (timeAdd.second + seconds) % 60;
      timeAdd = timeAdjustMinutes(timeAdd, multiple < 1 ? 1 : multiple + 1);
    }
  }
  else
  { // seconds are negative
    seconds = seconds * -1;
    if (seconds <= timeAdd.second)
    { // the seconds to be removed fit in this minute
      timeAdd.second = timeAdd.second - seconds;
    }
    else
    { // the seconds dont fit
      int multiple = seconds / 60;
      int remainder = seconds % 60;
      timeAdd.second = (timeAdd.second - seconds) % 60;
      timeAdd = timeAdjustMinutes(timeAdd, multiple < 1 ? -1 : ((-1 * multiple) - 1));
    }
  }
  return timeAdd;
}

_Time timeAdjustMinutes(_Time timeAdd, int minutes)
{
  if (minutes == 0)
    return timeAdd;
  if (minutes > 0)
  { // positive minutes
    if (minutes < (60 - timeAdd.minute))
    { // the minutes to be added fit in this minute
      timeAdd.minute = timeAdd.minute + minutes;
    }
    else
    { // the minutes dont fit
      int multiple = minutes / 60;
      int remainder = minutes % 60;
      timeAdd.minute = (timeAdd.minute + minutes) % 60;
      timeAdd = timeAdjustHours(timeAdd, multiple < 1 ? 1 : multiple + 1);
    }
  }
  else
  { // minutes are negative
    minutes = minutes * -1;
    if (minutes <= timeAdd.minute)
    { // the minutes to be removed fit in this minute
      timeAdd.minute = timeAdd.minute - minutes;
    }
    else
    { // the minutes dont fit
      int multiple = minutes / 60;
      int remainder = minutes % 60;
      timeAdd.minute = (timeAdd.minute - minutes) % 60;
      timeAdd = timeAdjustHours(timeAdd, multiple < 1 ? -1 : ((-1 * multiple) - 1));
    }
  }
  return timeAdd;
}

_Time timeAdjustHours(struct _Time timeAdd, int hours)
{
  // adjusting hours does not affect the higher up time variables - days etc.
  timeAdd.hour = (timeAdd.hour + hours) % 24;
  return timeAdd;
}
