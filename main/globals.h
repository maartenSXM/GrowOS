#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "global_Defines.h"
/******************************
 *        Global Variables
 ******************************/

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

extern TFT_eSPI tft;
extern FT6X36 ts;
extern DHT dht;

extern uint8_t XM;
extern uint8_t XP;
extern uint8_t YP;
extern uint8_t YM;
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
struct touchPoint_t
{
  uint16_t x;
  uint16_t y;
  uint16_t z;
  bool touched;
};
extern touchPoint_t tp;

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
extern const uint8_t daysPerMonth[numMonths + 1];

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
extern char strTemp[maxLabelLength];
extern char strTemp2[maxLabelLength];
extern char daysSelect[maxLabelLength];
extern char weeksSelect[maxLabelLength];
#define numDayBtns 8
extern uint8_t daysSelected[numDayBtns]; // 0th = All, 1th=Sunday     // 0=OFF 1=ON 2=HIDDEN
#define numWeekBtns 13
extern uint8_t weeksSelected[numWeekBtns]; // 0th = ALL, 1th = week1 .... 12=week12

extern ButtonWidget dayBtns[numDayBtns];
extern ButtonWidget weekBtns[numWeekBtns];
extern ButtonWidget daysSelectBtn;
extern ButtonWidget weeksSelectBtn;
extern ButtonWidget exitBtn;

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
extern int16_t currentTemp;
extern int16_t maxTempDay;
extern int16_t minTempDay;
extern int16_t targetTemp;
extern int16_t currentHumid;
extern int16_t maxHumidDay;
extern int16_t minHumidDay;
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

#endif // GLOBALS_H