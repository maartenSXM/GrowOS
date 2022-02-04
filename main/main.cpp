//file: main.c or main.cpp
#include "common.h"                        //Most config and global vars

#include "backupMode.h"
#include "buttons.h"
#include "flashStorage.h"
#include "gpio.h"
#include "helperFuncs.h"
#include "lcd.h"
#include "pages_scheduling.h"
#include "pages.h"
#include "rtc.h"
#include "schedule.h"
#include "timerFanControl.h"
#include "wifi_Funcs.h"

extern "C" {
    void app_main(void);
}

void app_main(void)
{
  initArduino();                          //needed for using arduino core in ESP-idf (git clone https://github.com/espressif/arduino-esp32.git arduino)
#ifdef DEBUG
  Serial.begin(9600);
#ifdef EnsureSerial
  while(!Serial);                               // wait for serial port to connect. Needed for native USB port only
#endif  
  Serial.println(F("DEBUG MODE"));
#endif
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );
  wifi_scan();

  digitalWrite(ACT_GND, true);                        //lcd uses ground switch
  digitalWrite(ACT_12V, true);
  delay(500);

  initializeLCD();
  
  dht.begin();

  loadGlobalButtons();

  if(enBUMODE){
    setupBUMODE();
  }
  
  //Reports the cause of the last processor reset. Reading this RSTC_SR does not reset this field.
  // 0 = fresh 1 = backup mode reset 2 = watchdog , 3 = software, 4 = User/NSRT
  //uint32_t resetStatus = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos;
  //For fresh starts only
  //if((resetStatus != 1) || (dueFlashStorage.read(0) == 255)) {
  if(false){
    //rtc.begin();
    //rtc.setTime(15, 59, 50);       //(hours,min,sec) Mil time
    //rtc.setDate(1, 3, 2021);      //day,month,year
    splashPage();                 //Welcome to opbox
    //configureTimePage();
    setNutrientDate();            //sets nutrient date as today.
    defaultDailySchedule();       //initialize the default schedule
  }
  //This will run if coming out of backup mode or non first start
  else {
    //rtc.begin();
    //rtc.setTime(15, 59, 50);      //(hours,min,sec) Mil time
    //rtc.setDate(1, 3, 2021);      //day,month,year
    delay(1);
    disablePageLimits();          //disable page limits that were used for this page but no longer valid for building next page from fresh
    splashPage();
    pollCurrentDate();            
    loadFlashSettings();          //retrieves nutrient date and week
    //configureTimePage();          //set time after power out-tage
  }
  
    //For first page we need to check the slot to poll current time and get new data to build homePage with
    readTempHumid();
    resetMinMaxTempHumid();
    checkSlot();
    homePage();

#ifdef DEBUG 
Serial.println(F("Setup Complete")); 
#endif

    loop();
}

void loop(){ 
    while(1){
    delay(1000);
    wifi_scan();
}}
