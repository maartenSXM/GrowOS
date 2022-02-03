#include "flashStorage.h"

void saveFlashSettings(){
  /*
  if(!enSaveToFlash) return;
  //settings saved bit
  dueFlashStorage.write(0,0);
  
  //sunrise hour
  dueFlashStorage.write(1,sunriseTime.hour);

  //hours on
  dueFlashStorage.write(2,daylight);

  //hours off
  dueFlashStorage.write(3,night);

  //set fan temp
  dueFlashStorage.write(4,targetTemp);

  //save fan speed
  dueFlashStorage.write(7,fanSpeed);
  */
}

void saveNutrientDateFlash(){
  /*
  if(!enSaveToFlash) return;
  dueFlashStorage.write(5,growDay);
  dueFlashStorage.write(6,growWeek);
  */
}

void loadFlashSettings(){
  /*
  tft.fillScreen(BLACK);
  if(dueFlashStorage.read(0) != 0){
    tft.print("No settings to read",20,20);
    delay(1000);
    return;                //no settings to read
  }

  tft.setTextSize(2);
  tft.setFont(NULL);
  tft.setColor(RED,BLACK);

  tft.print("WARNING",xCenter-40,60); 

  tft.setColor(WHITE,BLACK);
  
  tft.print("GROW LIGHT WILL NOW BE ACTIVATED",45,90); 

  tft.setTextSize(1);
  tft.setFont(NULL);
  
  tft.print("RECOVERED SETTINGS",xCenter-52,yDisp-120); 
  
  //sunrise hour
  sunriseTime.hour = dueFlashStorage.read(1);
  tft.print("Sunrise: " + String(sunriseTime.hour),110,yDisp-80); 

  //hours on
  daylight = dueFlashStorage.read(2);
  tft.print("Daylight Hours: " + String(daylight),110,yDisp-60); 
  
  //hours off
  night = dueFlashStorage.read(3);
  tft.print("Night Hours: " + String(night),110,yDisp-40);

  //set fan temp
  targetTemp = dueFlashStorage.read(4);
  tft.print("Fan Target Temp: " + String(targetTemp),xCenter+20,yDisp-80);

  //day and week of grow
  growDay = dueFlashStorage.read(5);
  if(growDay == 255) growDay = 1;
  tft.print("Current Grow Day: " + String(growDay),xCenter+20,yDisp-60);
  growWeek = dueFlashStorage.read(6);
  if(growWeek == 255) growWeek = 1;
  tft.print("Current Grow Week: " + String(growWeek),xCenter+20,yDisp-40);

  // fan speed
  fanSpeed = dueFlashStorage.read(7);

  //load currentDate with current date so we can set prevDayOfWeek to be same and not instantly roll over to a new day the next time the date/time is checked.
  int dayOfWeek, day, month = 0;
  //rtc.getDate(&dayOfWeek,&day,&month,&currentDate.year);    //day_of_week, day, month, year
  currentDate.dayOfWeek = uint8_t(dayOfWeek);
  currentDate.day = uint8_t(day);
  currentDate.month = uint8_t(month);
  prevDayOfWeek = currentDate.dayOfWeek;

  //SAVE
  saveSlotTimes();          //requires  sunriseTime.hour and daylight to be set

  delay(5000);
  //Start new schedule
  findCurrentSlot();
  startNewSlot();
  */
}
