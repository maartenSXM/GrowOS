/******************************
 *        Global Variables
 ******************************/

#ifndef GLOBALS_H_
#define GLOBALS_H_

/***** EXT LIBS CONFIG *****/
#include <Arduino.h>     //for basic shit
#include <Wire.h>        //I2C for CTP touch controller
#include <FT6X36.h>      //Touch Controller
#include "SPI.h"         //SPI for Display
#include "TFT_eSPI.h"    //The API for controlling the TFT
#include "driver/ledc.h" //PWM for brightness
#include "DHT.h"         //TEMP & HUMID Sensor

#include "esp_system.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include <time.h>

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

#include "./Fonts/FreeSans12pt7b.h" // Font files to include in library
#include "./Fonts/FreeSans9pt7b.h"
#include "./Fonts/FreeMonoBoldOblique24pt7b.h"
#include "./Fonts/FreeMonoBold24pt7b.h"
#include "./Fonts/FreeSansBold24pt7b.h"
#include "./Fonts/FreeSans24pt7b.h"

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

// HARDWARE CONFIG

#define H_GPIO 0
#define CSI_VSYNC 1
#define CSI_D9 2
#define CSI_D8 3
#define CSI_D7 4
#define CSI_D6 5
#define CTP_IRQ 6
#define CTP_RST 7
#define TFT_RST 7
#define TWI_SDA 8
#define TWI_SCK 9
#define CSI_PWR_EN 10
#define CSI_PCLK 11
#define CSI_D4 12
#define H_S_ENABLE 13
#define PWROFF_INT 14
#define TEMP_DATA 17
#define TFT_PWR_EN 18
#define H_USB -19
#define H_USB +20
#define TFT_BACKLIGHT 21
#define TFT_CS 33
#define CSI_HREF 33
#define TFT_DC 34
#define CSI_XCLK 34
#define TFT_MOSI 35
#define CSI_D2 35
#define TFT_SCLK 36
#define CSI_D3 36
#define TFT_MISO 37
#define CSI_D5 37
#define VBAT_SENSE 38
#define H_GPIO39 39
#define H_GPIO40 40
#define H_GPIO41 41
#define H_GPIO42 42
#define H_U0TXD
#define H_U0RXD
#define VBAT_CE 45

#define FT6236_TOUCH_FREQUENCY 400000

#define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE

// 565 colours http://www.barth-dev.de/online/rgb565-color-picker/
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define BLACK 0x0000       /*   0,   0,   0 */
#define OFFBLACK 0x2124    /*  38,  38,  38 */
#define DARKGREY 0x7BEF    /* 128, 128, 128 */
#define GREY 0x9CF3        /* 156, 156, 156 */
#define LIGHTGREY 0xC618   /* 192, 192, 192 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define RED 0xF800         /* 255,   0,   0 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define ORANGE 0xFDA0      /* 255, 180,   0 */
#define GREENYELLOW 0xB7E0 /* 180, 255,   0 */
#define PINK 0xFC9F
#define OPBOX_GREEN 0x7DE8

#define offOutline GREY
#define offFill BLACK
#define textColour WHITE
#define onOutline GREEN
#define onFill DARKGREEN
#define hiddenTextcolor DARKGREY

#define secondsInDay 86400
const u_int8_t daysPerMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define growOSSizeOfDeviceLabel 20

/***** Global Vars *****/

/***** GENERAL SOFTWARE CONFIG *****/
// #define Serial SerialUSB                              //native uses SerialUSB
// #define DEBUG                                         //for serial debugging and other
// #define DEBUGTOUCH                                    //shows boxes around items that can be touched
// #define EnsureSerial                                  //waits for serial port to be ready
// #define DEVELOPMENT_MODE                              //settings to use when developing
// #define saveToFlash                                   //to enable saving important settings to flash and recovering them after power off.

// Uncomment whatever type you're using!
// #define DHTTYPE DHT22
#define DHTTYPE DHT21 // DHT 21 (AM2301)     //DHTTYPE DHT11 = DHT 11  // DHT 22  = (AM2302), AM2321

#ifdef DEVELOPMENT
// RTCDue rtc(RC);
#else
// #define Serial SerialUSB
// RTCDue rtc(XTAL);     // Select the HW Slowclock source
#endif

#define PORTRAIT 0
#define LANDSCAPE 1

#define maxNumSlots 100

#define numDayBtns 8
#define numWeekBtns 13

extern const char *TAG;

/***** WIFI CONFIG *****/
extern char SSIDName[33];
extern char PasswordAP[65];
extern IPAddress esp32IP;
extern char hostName[33];

extern uint8_t ESP32_BASE_MAC_ADDRESS[6];
// extern WebServer server;

/***** FIREBASE *****/
// See passwords.h for more
extern char FIREBASE_FILE_PHOTO_PATH[65];

extern UTFTGLUE tft;
extern TouchScreen ts;
extern TSPoint tp; // the coordinates are stored here during a touch
extern DHT dht;
extern uint32_t cx, cy, cz;
extern uint32_t rx[8], ry[8];
extern int32_t clx, crx, cty, cby;
extern float px, py;
extern int xDisp, yDisp, xCenter, yCenter, swapxy;
extern uint32_t calx, caly, cals;
extern uint16_t TOUCH_ORIENTATION;
extern uint16_t maxPressureThreshold;
extern bool touchCalibrated;
// default calibration for 3.5" ILI 0x9486 display
extern uint16_t CAL_TS_LEFT, CAL_TS_RT, CAL_TS_TOP, CAL_TS_BOT; // Touch screen calibration
extern uint16_t CAL_TS_WID, CAL_TS_HT;                          // Touch screen calibration, in landscape mode WID = 480 HT=320
// uint16_t TOUCH_ORIENTATION  PORTRAIT
// uint16_t CAL_TS_LEFT = 934, CAL_TS_RT = 138, CAL_TS_TOP= 840, CAL_TS_BOT= 144;  //Touch screen calibration
// uint16_t CAL_TS_WID = 480, CAL_TS_HT = 320;
extern uint16_t X_Coord, Y_Coord; // Note these should be same as tp.x and tp.y temp global for x and y coord of a touch screen press
extern uint32_t timeLastTouch;
extern uint32_t lcdTimeoutDuration;
extern bool lcdSleep;

struct _Date
{
  uint8_t dayOfWeek;
  uint8_t day;
  uint8_t month;
  uint16_t year; // according to library has to be 16bit
};
struct _Time
{
  int32_t hour;
  int32_t minute;
  int32_t second;
};
struct _DateTime
{
  int32_t dayOfWeek;
  int32_t day;
  int32_t month;
  int32_t year;
  int32_t hour;
  int32_t minute;
  int32_t second;
};

extern struct tm timeInfo; // Global variable to hold time information
extern struct timeval tv_now;
extern _Date nutrientDate;
extern _Date currentDate;
extern _Date dateTemp;
extern _Time sunriseTime;
extern _Time sunsetTime;
extern _Time sleepTime;
extern _Time currentTime;
extern _Time newTSTime;
extern _Time timeTemp;
extern uint8_t growWeek;
extern uint8_t growDay;
extern uint8_t daylight; // hours of daylight
extern uint8_t night;    // hours of night
extern int yTime;

// devIs coding:
// 0:fan 1:light 2:pump 3:12vaux 4:5vaux 5:redLight HS switch 6:Special 0
extern uint8_t devicePins[numDevices];
extern int8_t numSlots;
extern int8_t currentSlot;
extern int week;
extern int prevDayOfWeek; // saves the previous day of week so we know when we have rolled to a new one.
#define maxNumSlots 100
// ScheduleBlocks - sets blocks of time where certain devices to be set to a certain state
// 1:off, 2:on, 3:auto(default) - device chooses when to be on to meet plants needs, 4:periodic off/on.
// lets give 8 slots, left to right they are earliest to latest. If the clock finds itself outside a block it is always set to auto
extern bool devIsAuto[maxNumSlots][numDevices];          // overrides all below component modes is the device on auto mode,
extern bool devIsState[maxNumSlots][numDevices];         // manually set what is active in that block - 0th bit = fan 1=light 2=pump...
extern bool devIsPeriodic[maxNumSlots][numDevices];      // is device in periodic mode for this block, componentIsState must be set to true
extern uint16_t devPeriodic[maxNumSlots][numDevices][4]; // 0th is off duration, 1st is on duration, 2nd is state, 3rd is sinceStateChange
extern _Time startTimeSlots[maxNumSlots];                // the begining of a schedule slot, index zero is default sunrise start, index numSlots -1 is sleep start
extern bool customApplication;                           // set to true to use the technicalNames, and when custom names for devices are needed.
extern char hydroponicDeviceNames[numDevices][growOSSizeOfDeviceLabel];
extern char technicalDeviceNames[numDevices][growOSSizeOfDeviceLabel];
extern char deviceNames[numDevices][growOSSizeOfDeviceLabel];
extern char strTemp[_ADA_GFX_sizeOfLabel];
extern char strTemp2[_ADA_GFX_sizeOfLabel];
extern char daysSelect[_ADA_GFX_sizeOfLabel];
extern char weeksSelect[_ADA_GFX_sizeOfLabel];
#define numDayBtns 8
extern uint8_t daysSelected[numDayBtns]; // 0th = All, 1th=Sunday     // 0=OFF 1=ON 2=HIDDEN
#define numWeekBtns 13
extern uint8_t weeksSelected[numWeekBtns]; // 0th = ALL, 1th = week1 .... 12=week12

extern Adafruit_GFX_Button dayBtns[numDayBtns];
extern Adafruit_GFX_Button weekBtns[numWeekBtns];
extern Adafruit_GFX_Button daysSelectBtn;
extern Adafruit_GFX_Button weeksSelectBtn;
extern Adafruit_GFX_Button exitBtn;

// number of seconds for device to be on in auto settings
extern uint16_t fanQuota;     // 18 hours
extern uint16_t lightQuota;   // 18 hours
extern uint16_t pumpOffTime;  // time to be off for
extern uint16_t pumpOnTime;   // time to be on for
extern uint16_t twelveVQuota; // 18 hours
extern uint16_t fiveVQuota;   // 18 hours
extern uint16_t HS_AUX_STATE;
extern uint16_t fanSpeed;
extern bool c; // show temp in celcius or fahrenheit
extern float currentTemp;
extern float maxTempDay;
extern float minTempDay;
extern float targetTemp;
extern float currentHumid;
extern float maxHumidDay;
extern float minHumidDay;
extern int timePumpOn;  // seconds
extern int timePumpOff; // seconds 1800=30min
extern int PWMFrequency;
extern int currentPage;  // start at homePage
extern bool checkTPOnly; // when building a page if checkTPOnly == true, do not rebuild the page only check that page for a relevant touch.
extern bool updatePageOnly;
extern bool reDrawStuff;
extern bool enBUMODE;
extern bool gotPWROFF_INT;
extern bool enSaveToFlash;
extern char buf[40];

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

#endif // GLOBALS_H