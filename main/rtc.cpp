#include <sys/time.h>

// Function to initialize time zone for Toronto
void initializeTimeZone()
{
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1); // Eastern Time with daylight saving time (Toronto)
  tzset();
}

void pollCurrentTime()
{
  gettimeofday(&tv_now, NULL);

  localtime_r(&tv_now.tv_sec, &timeInfo);

  // Extract the hour, minute, and second from the timeinfo structure
  int32_t hour = timeInfo.tm_hour;
  int32_t minute = timeInfo.tm_min;
  int32_t second = timeInfo.tm_sec;

  // Assign the values to the currentTime structure (assuming currentTime is defined)
  currentTime.hour = hour;
  currentTime.minute = minute;
  currentTime.second = second;
}

void pollCurrentDate()
{
  gettimeofday(&tv_now, NULL);

  localtime_r(&tv_now.tv_sec, &timeinfo);

  int32_t dayOfWeek = timeinfo.tm_wday;   // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
  int32_t day = timeinfo.tm_mday;         // Day of the month (1 - 31)
  int32_t month = timeinfo.tm_mon + 1;    // Month (0 - 11, so add 1 to get 1 - 12)
  int32_t year = timeinfo.tm_year + 1900; // Year since 1900

  currentDate.dayOfWeek = dayOfWeek;
  currentDate.month = month;
  currentDate.day = day;
  currentDate.year = year;

  if (prevDayOfWeek != currentDate.dayOfWeek)
  { // its a new day!
    growDay += 1;
    prevDayOfWeek = currentDate.dayOfWeek;
    maxHumidDay = currentHumid;
    minHumidDay = currentHumid;
    maxTempDay = currentTemp;
    minTempDay = currentTemp;
    if (currentDate.dayOfWeek == nutrientDate.dayOfWeek)
    { // its a new day on the week-versary of starting the grow
      growWeek += 1;
      growDay = 1;
    }
    // save to flash so we can restart on same grow week and day, note SAM3X8E can handle 10k @85c writes before not guaranteed,
    // saveNutrientDateFlash();
  }
}

void setNutrientDate()
{
  pollCurrentDate();

  nutrientDate.dayOfWeek = currentDate.dayOfWeek;
  nutrientDate.day = currentDate.day;
  nutrientDate.month = currentDate.month;
  nutrientDate.year = currentDate.year;

  prevDayOfWeek = nutrientDate.dayOfWeek;
  growWeek = 1;
  growDay = 1;
}

uint16_t getDaysSinceNutrientDate()
{
  if (nutrientDate.year == 0)
  {            // year is set to zero
    return -1; // nutrients have not been set yet
  }
  pollCurrentDate();
  return dateDiff(nutrientDate.year, nutrientDate.month, nutrientDate.day, currentDate.year, currentDate.month, currentDate.day);
}

int dateDiff(int year1, int mon1, int day1, int year2, int mon2, int day2)
{
  int ref, dd1, dd2, i;
  ref = year1;
  if (year2 < year1)
    ref = year2;
  dd1 = 0;
  dd1 = dater(mon1);
  for (i = ref; i < year1; i++)
  {
    if (i % 4 == 0)
      dd1 += 1;
  }
  dd1 = dd1 + day1 + (year1 - ref) * 365;
  dd2 = 0;
  for (i = ref; i < year2; i++)
  {
    if (i % 4 == 0)
      dd2 += 1;
  }
  dd2 = dater(mon2) + dd2 + day2 + ((year2 - ref) * 365);
  return dd2 - dd1;
}

int dater(int x)
{
  const int dr[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  return dr[x - 1];
}
