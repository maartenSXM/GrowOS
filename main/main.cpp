#include "global_Defines.h"

/***** EXT LIBS CONFIG *****/
#include "Arduino.h" //for basic shit
#include "esp_system.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp32s2/rom/rtc.h"
#include "time.h"
#include "sntp.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>
#include "esp_event.h"
#include "nvs_flash.h"

#include <WiFi.h> //Include WiFi library
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "soc/soc.h"          // Disable brownout problems
#include "soc/rtc_cntl_reg.h" // Disable brownout problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include <FS.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h> //Provide the token generation process info.
#include <HTTPUpdate.h>         //for firmware update

// file: main.c or main.cpp
#include "globals.h" //Most config and global vars

/***** App Code Src Headers *****/
#include "passwords.h"
#include "icons.h" //Bitmap images
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
#include "firebase_Funcs.h"
#include "esp32SystemAPIHandler.h"

void setup(void)
{
#ifdef DEBUG
  Serial.begin(115200);
#ifdef EnsureSerial
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB port only
  Serial.println("waiting for serial");
  for (int i = 0; i < 6; i++)
  {
    Serial.println(String(10 - i));
    delay(1000);
  }
#endif
  ESP_LOGI(TAG, "IN DEBUG MODE");
#endif
  //  Disable the timer watchdogs,
  // NOT WORKING - DISABLED IN sdkconfig instead
  // ESP_ERROR_CHECK(esp_task_wdt_deinit());

  // Get the last reset reason
  uint8_t reset_reason = rtc_get_reset_reason(0);
  switch (reset_reason)
  {
  case 1:
    Serial.println("Vbat power on reset");
    break;
  case 3:
    Serial.println("Software reset digital core");
    break;
  case 4:
    Serial.println("Legacy watch dog reset digital core");
    break;
  case 5:
    Serial.println("Deep Sleep reset digital core");
    break;
  case 6:
    Serial.println("Reset by SLC module, reset digital core");
    break;
  case 7:
    Serial.println("Timer Group0 Watch dog reset digital core");
    break;
  case 8:
    Serial.println("Timer Group1 Watch dog reset digital core");
    break;
  case 9:
    Serial.println("RTC Watch dog Reset digital core");
    break;
  case 10:
    Serial.println("Instrusion tested to reset CPU");
    break;
  case 11:
    Serial.println("Time Group reset CPU");
    break;
  case 12:
    Serial.println("Software reset CPU");
    break;
  case 13:
    Serial.println("RTC Watch dog Reset CPU");
    break;
  case 14:
    Serial.println("for APP CPU, reseted by PRO CPU");
    break;
  case 15:
    Serial.println("Reset when the vdd voltage is not stable");
    break;
  case 16:
    Serial.println("RTC Watch dog reset digital core and rtc module");
    break;
  default:
    Serial.println("NO_MEAN");
  }

  // Call the function to initialize the time zone for Toronto
  initializeTimeZone();
  // Get the current time from the RTC
  time_t now;
  struct tm timeinfo;

  set_rtc_time();
  pollCurrentDateTime();

  // Initialize Touch Controller
  Wire.begin();
  ESP_LOGI(TAG, "Initializing touch controller");
  ts.begin();
  ts.registerTouchHandler(touchHandler); //.registerTouchHandler(touch); uses a polling method via ts.loop
  // ts.registerIsrHandler(touchHandler);  //.registerIsrHandler(touch); uses an interrupt method via Interrupt Service Routine
  ESP_LOGI(TAG, "Touch controller initialized");

  // Initialize LCD display
  initializeLCD();

  dht.begin();

  loadGlobalButtons();

  if (reset_reason == 1)
  {
    // The reset reason is "ESP_RST_POWERON"

    splashPage(); // Welcome to opbox
    // configureTimePage();
    setNutrientDate();      // sets nutrient date as today.
    defaultDailySchedule(); // initialize the default schedule
  }
  else
  {
    // The reset reason is not "ESP_RST_POWERON"

    disablePageLimits(); // disable page limits that were used for this page but no longer valid for building next page from fresh
    splashPage();
    pollCurrentDate();
    loadFlashSettings(); // retrieves nutrient date and week
    resetSlot();
    // configureTimePage();          //set time after power out-tage
  }

  ESP_LOGI(TAG, "OUT of splashPage");
  //
  //// Initialize NVS
  // esp_err_t ret = nvs_flash_init();
  // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  //{
  //   ESP_ERROR_CHECK(nvs_flash_erase());
  //   ret = nvs_flash_init();
  // }
  // ESP_ERROR_CHECK(ret);

  // wifi_scan();

  // if (enBUMODE)
  //{
  //   setupBUMODE();
  // }

  // For first page we need to check the slot to poll current time and get new data to build homePage with
  readTempHumid();
  resetMinMaxTempHumid();
  checkSlot();
  // homePage();

#ifdef DEBUG
  Serial.println(F("Setup Complete"));
#endif
}

void loop()
{
#ifdef DEBUG
  Serial.println("*****New Loop*****\nTime: " + String(currentTime.hour) + ":" + String(currentTime.minute) + " " + String(currentTime.second));
  debugSchedule();
#endif
  // is lcd pressed? if yes -> handle the button and come back
  waitForTouch(950); // wait for a touch for n milliseconds

  // check the time, and do scheduled tasks
  checkSlot();

  // rebuild pages with new variable data such as time and external device settings, Note: this function will not build pages from scratch, it sets the bool "updatePageOnly"
  updatePage();

  // check if lcd has not been touched in a while
  lcdTimeout();
}
