#include "rtc.h"

void pollCurrentTime(){
  /*
  int hour, minute, second;
  //rtc.getTime(&hour,&minute,&second);
  currentTime.hour = uint8_t(hour);
  currentTime.minute = uint8_t(minute);
  currentTime.second = uint8_t(second);
  */
}

void pollCurrentDate(){
  /*
  int dayOfWeek, day, month;
  rtc.getDate(&dayOfWeek,&day,&month,&currentDate.year);    //day_of_week, day, month, year
  currentDate.dayOfWeek = uint8_t(dayOfWeek);
  currentDate.day = uint8_t(day);
  currentDate.month = uint8_t(month);
  
  if(prevDayOfWeek != currentDate.dayOfWeek){               //its a new day!
    growDay += 1;
    prevDayOfWeek = currentDate.dayOfWeek;
    maxHumidDay = currentHumid;
    minHumidDay = currentHumid;
    maxTempDay = currentTemp;
    minTempDay = currentTemp;
    if(currentDate.dayOfWeek == nutrientDate.dayOfWeek){    //its a new day on the week-versary of starting the grow
      growWeek += 1;
      growDay = 1;
    }
    //save to flash so we can restart on same grow week and day, note SAM3X8E can handle 10k @85c writes before not guaranteed, 
    saveNutrientDateFlash();
  }
  */
}

void setNutrientDate(){
  /*
  int dayOfWeek, day, month;
  rtc.getDate(&dayOfWeek,&day,&month,&nutrientDate.year);    //day_of_week, day, month, year
  nutrientDate.dayOfWeek = uint8_t(dayOfWeek);
  nutrientDate.day = uint8_t(day);
  nutrientDate.month = uint8_t(month);
  prevDayOfWeek= nutrientDate.dayOfWeek;
  growWeek = 1;
  growDay = 1;
  */
}

uint16_t getDaysSinceNutrientDate(){
  if(nutrientDate.year == 0){   //year is set to zero
    return -1;                  //nutrients have not been set yet
  }
  pollCurrentDate();
  return dateDiff(nutrientDate.year, nutrientDate.month, nutrientDate.day, currentDate.year, currentDate.month, currentDate.day);
}

int dateDiff(int year1, int mon1, int day1, int year2, int mon2, int day2)
{
    int ref,dd1,dd2,i;
    ref = year1;
    if(year2<year1) ref = year2;
    dd1=0;
    dd1=dater(mon1);
    for(i=ref;i<year1;i++)
    {
        if(i%4==0)
        dd1+=1;
    }
    dd1=dd1+day1+(year1-ref)*365;
    dd2=0;
    for(i=ref;i<year2;i++)
    {
        if(i%4==0)
        dd2+=1;
    }
    dd2=dater(mon2)+dd2+day2+((year2-ref)*365);
    return dd2-dd1;
}

int dater(int x)
{ const int dr[]= { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  return dr[x-1];
}
