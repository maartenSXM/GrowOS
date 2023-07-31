#include <sys/time.h>
#include "rtc.h"

esp_err_t init_rtc()
{
  esp_err_t ret = ESP_OK;
  ret = rtc_clk_slow_freq_set(RTC_SLOW_FREQ_32K_XTAL); // Set RTC clock source to external 32kHz crystal
                                                       // Check if there was an error during initialization
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "RTC initialization failed with error code %d", ret);
    // Handle the error or take appropriate actions
  }
  else
  {
    // RTC initialization successful
    ESP_LOGI(TAG, "RTC initialized successfully");
    // Proceed with your application logic
  }

  // No need to set CPU frequency, as it's configured in sdkconfig.h and cannot be changed at runtime
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

  return ret;
}

// Optional function parameters
void set_rtc_time(int year, int month, int day, int hour, int minute, int second)
{
  struct tm timeinfo;
  timeinfo.tm_year = year - 1900; // Year (current year - 1900)
  timeinfo.tm_mon = month - 1;    // Month (0-based, January is 0)
  timeinfo.tm_mday = day;         // Day of the month (1 to 31)
  timeinfo.tm_hour = hour;        // Hours (0 to 23)
  timeinfo.tm_min = minute;       // Minutes (0 to 59)
  timeinfo.tm_sec = second;       // Seconds (0 to 59)

  time_t timestamp = mktime(&timeinfo); // Convert struct tm to a UNIX timestamp

  esp_err_t ret = rtc_set_time(timestamp);
  if (ret != ESP_OK)
  {
    ESP_LOGE(TAG, "Failed to set RTC time");
  }
}

// Function to initialize time zone for Toronto
void initializeTimeZone()
{
  setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1); // Eastern Time with daylight saving time (Toronto)
  tzset();
}

void pollCurrentDateTime()
{
  struct tm timeinfo;

  gettimeofday(&tv_now, NULL);

  localtime_r(&tv_now.tv_sec, &timeInfo);

  // Extract the hour, minute, and second from the timeinfo structure
  int32_t dayOfWeek = timeinfo.tm_wday;   // 0 = Sunday, 1 = Monday, ..., 6 = Saturday
  int32_t day = timeinfo.tm_mday;         // Day of the month (1 - 31)
  int32_t month = timeinfo.tm_mon + 1;    // Month (0 - 11, so add 1 to get 1 - 12)
  int32_t year = timeinfo.tm_year + 1900; // Year since 1900
  int32_t hour = timeInfo.tm_hour;
  int32_t minute = timeInfo.tm_min;
  int32_t second = timeInfo.tm_sec;

  // Assign the values to the currentTime structure (assuming currentTime is defined)
  currentDate.dayOfWeek = dayOfWeek;
  currentDate.month = month;
  currentDate.day = day;
  currentDate.year = year;
  currentTime.hour = hour;
  currentTime.minute = minute;
  currentTime.second = second;
}

void pollCurrentTime()
{
  struct tm timeinfo;

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
  struct tm timeinfo;

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
