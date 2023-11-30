/* esp-idf headers */

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <sdkconfig.h>
#include <esp_log.h>
#include <esp_err.h>

/* C libraries */

#include <stdio.h>

/* local driver headers */

#include "time.h"
#include "soc/soc.h"          // Disable brownout problems
#include "soc/rtc_cntl_reg.h" // Disable brownout problems
#include "esp32s2/rom/rtc.h"

/***** application headers *****/

#include "buttons.h"
#include "flashStorage.h"
#include "gpio.h"
#include "lcd.h"
#include "pages.h"
#include "rtc.h"
#include "schedule.h"

#if 0 /* NOT_NEEDED */

#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "global_Defines.h"
#include "Arduino.h" //for basic shit
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "esp_system.h"
#include "esp_sntp.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <esp_wifi.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include <WiFi.h> //Include WiFi library
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include <FS.h>
#include <HTTPUpdate.h>         //for firmware update
#include "globals.h" //Most config and global vars
#include "icons.h" //Bitmap images
#include "backupMode.h"
#include "helperFuncs.h"
#include "pages_scheduling.h"
#include "timerFanControl.h"
#include "wifi_Funcs.h"
#include "esp32SystemAPIHandler.h"

#endif /* NOT_NEEDED */

/* defines to move to kconfig.proj */

/* https://github.com/espressif/esp-idf/blob/release/v4.4/examples/peripherals/uart/uart_echo/main/Kconfig.projbuild */
#define GB_UART_BUF_SIZE 256
#define GB_UART_BAUD_RATE 115200
#define GB_UART_PORT_NUM 1
#define GB_UART_RXD_PIN	 5
#define GB_UART_TXD_PIN	 4
#define GB_UART_RTS_PIN	 UART_PIN_NO_CHANGE
#define GB_UART_CTS_PIN	 UART_PIN_NO_CHANGE

void startCPU1()
  {
#define GB_AUX_CPU_PIN GPIO_NUM_13

#if 0
#define GPIO_START_CPU1_PIN  (1ULL << GB_AUX_CPU_PIN)
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_START_CPU1_PIN;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    gpio_config(&io_conf);
#endif
    gpio_reset_pin(GB_AUX_CPU_PIN);
    gpio_set_direction(GB_AUX_CPU_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GB_AUX_CPU_PIN, 1);
  }

int dontoptimizemeout = 0;
void haltCPU0()
  {
  dontoptimizemeout = 1;
  }

void setup(void)
{
#ifdef DEBUG
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
    printf("Vbat power on reset\n");
    break;
  case 3:
    printf("Software reset digital core\n");
    break;
  case 4:
    printf("Legacy watch dog reset digital core\n");
    break;
  case 5:
    printf("Deep Sleep reset digital core\n");
    break;
  case 6:
    printf("Reset by SLC module, reset digital core\n");
    break;
  case 7:
    printf("Timer Group0 Watch dog reset digital core\n");
    break;
  case 8:
    printf("Timer Group1 Watch dog reset digital core\n");
    break;
  case 9:
    printf("RTC Watch dog Reset digital core\n");
    break;
  case 10:
    printf("Instrusion tested to reset CPU\n");
    break;
  case 11:
    printf("Time Group reset CPU\n");
    break;
  case 12:
    printf("Software reset CPU\n");
    break;
  case 13:
    printf("RTC Watch dog Reset CPU\n");
    break;
  case 14:
    printf("for APP CPU, reseted by PRO CPU\n");
    break;
  case 15:
    printf("Reset when the vdd voltage is not stable\n");
    break;
  case 16:
    printf("RTC Watch dog reset digital core and rtc module\n");
    break;
  default:
    printf("NO_MEAN\n");
  }

  // Call the function to initialize the time zone for Toronto
  initializeTimeZone();
  // Get the current time from the RTC
  // time_t now;
  // struct tm timeinfo;

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

 startCPU1();
 haltCPU0();

#ifdef DEBUG
  printf("Setup Complete\n");
#endif
}

void loop()
{
  haltCPU0();

#ifdef DEBUG
  printf("*****New Loop*****\n");
  printf("Time: %d:%d:%d\n", currentTime.hour, currentTime.minute,
							currentTime.second);
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
