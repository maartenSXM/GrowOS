// file: main.c or main.cpp
#include "globals.h" //Most config and global vars

extern "C"
{
  void app_main(void);
}

void app_main(void)
{
  initArduino(); // needed for using arduino core in ESP-idf (git clone https://github.com/espressif/arduino-esp32.git arduino)
#ifdef DEBUG
  Serial.begin(9600);
#ifdef EnsureSerial
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB port only
#endif
  Serial.println(F("DEBUG MODE"));
#endif
  // Call the function to initialize the time zone for Toronto
  initializeTimeZone();
  // Get the current time from the RTC
  time_t now;
  struct tm timeinfo;
  if (esp_sleep_get_time(&now))
  {
    localtime_r(&now, &timeinfo);
  }
  else
  {
    ESP_LOGE("RTC", "Failed to get time from RTC");
    return;
  }

  // Print the current time
  char strftime_buf[64];
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI("RTC", "Current time: %s", strftime_buf);

  // Get the last reset reason
  esp_reset_reason_t reset_reason = esp_reset_reason();
  const char *reset_reason_str = esp_reset_reason_get_name(reset_reason);

  // Print the reset reason
  ESP_LOGI("RESET_REASON", "Last reset reason: %s", reset_reason_str);

  if (strcmp(reset_reason_str, "ESP_RST_POWERON") == 0)
  {
    // The reset reason is "ESP_RST_POWERON"
    // Your code here...
  }
  else
  {
    // The reset reason is not "ESP_RST_POWERON"
    // Your code here...
  }

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_scan();

  // Initialize Touch Controller
  Wire.begin();
  ts.begin();
  ts.registerTouchHandler(touch);

  // Initialize LCD display
  initializeLCD();

  dht.begin();

  loadGlobalButtons();

  if (enBUMODE)
  {
    setupBUMODE();
  }

  // Reports the cause of the last processor reset. Reading this RSTC_SR does not reset this field.
  //  0 = fresh 1 = backup mode reset 2 = watchdog , 3 = software, 4 = User/NSRT
  uint32_t resetStatus = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos;
  // For fresh starts only
  if ((resetStatus != 1) || (dueFlashStorage.read(0) == 255))
  {
    rtc.begin();
    rtc.setTime(15, 59, 50); //(hours,min,sec) Mil time
    rtc.setDate(1, 3, 2021); // day,month,year
    splashPage();            // Welcome to opbox
    // configureTimePage();
    setNutrientDate();      // sets nutrient date as today.
    defaultDailySchedule(); // initialize the default schedule
  }
  // This will run if coming out of backup mode or non first start
  else
  {
    rtc.begin();
    rtc.setTime(15, 59, 50); //(hours,min,sec) Mil time
    rtc.setDate(1, 3, 2021); // day,month,year
    delay(1);
    disablePageLimits(); // disable page limits that were used for this page but no longer valid for building next page from fresh
    splashPage();
    pollCurrentDate();
    loadFlashSettings(); // retrieves nutrient date and week
    // configureTimePage();          //set time after power out-tage
  }

  // Reports the cause of the last processor reset. Reading this RSTC_SR does not reset this field.
  //  0 = fresh 1 = backup mode reset 2 = watchdog , 3 = software, 4 = User/NSRT
  // uint32_t resetStatus = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos;
  // For fresh starts only
  // if((resetStatus != 1) || (dueFlashStorage.read(0) == 255)) {
  bool firstBoot = true;

  if (firstBoot)
  {
    splashPage();
    initialProvision();
  }
  if (false)
  {
    // rtc.begin();
    // rtc.setTime(15, 59, 50);      //(hours,min,sec) Mil time
    // rtc.setDate(1, 3, 2021);      //day,month,year
    splashPage(); // Welcome to CyberGrow!
    // configureTimePage();
    setNutrientDate();      // sets nutrient date as today.
    defaultDailySchedule(); // initialize the default schedule
  }
  // This will run if coming out of backup mode or non first start
  else
  {
    // rtc.begin();
    // rtc.setTime(15, 59, 50);      //(hours,min,sec) Mil time
    // rtc.setDate(1, 3, 2021);      //day,month,year
    delay(1);
    disablePageLimits(); // disable page limits that were used for this page but no longer valid for building next page from fresh
    splashPage();
    pollCurrentDate();
    loadFlashSettings(); // retrieves nutrient date and week
    // configureTimePage();          //set time after power out-tage
  }

  // For first page we need to check the slot to poll current time and get new data to build homePage with
  readTempHumid();
  resetMinMaxTempHumid();
  checkSlot();
  homePage();

#ifdef DEBUG
  Serial.println(F("Setup Complete"));
#endif

  loop();
}

void loop()
{
  while (1)
  {
#ifdef DEBUG
    Serial.println("*****New Loop*****\nTime: " + String(currentTime.hour) + ":" + String(currentTime.minute) + " " + String(currentTime.second));
    debugSchedule();
#endif
    // is lcd pressed? if yes -> handle the button and come back
    waitForTouch(900); // wait for a touch for n milliseconds

    // check the time, and do scheduled tasks
    checkSlot();

    // rebuild pages with new variable data such as time and external device settings, Note: this function will not build pages from scratch, it sets the bool "updatePageOnly"
    updatePage();

    // check if lcd has not been touched in a while
    lcdTimeout();
  }
}
