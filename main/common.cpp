#include "common.h"

const char *TAG = "growos";

UTFTGLUE tft(0x9468, LCD_CD, LCD_WR, LCD_CS, LCD_RESET, LCD_RD);    //MCUFRIEND_kbv tft;
TouchScreen ts(1, 2, 3, 4, 290);                                    //re-initialised after diagnose, the final param is the resistance across x- x+
TSPoint  tp;                                                       //the coordinates are stored here during a touch
DHT dht(DHTPIN, DHTTYPE);                                           //init DHT sensor

char SSIDName[33];
char PasswordAP[65];

uint8_t XM = 1;
uint8_t XP = 8;
uint8_t YP = 2;
uint8_t YM = 9;

uint32_t cx, cy, cz;
uint32_t rx[8], ry[8];
int32_t clx, crx, cty, cby;
float px, py;
int xDisp, yDisp, xCenter, yCenter, swapxy;
uint32_t calx, caly, cals;

uint16_t TOUCH_ORIENTATION = LANDSCAPE;
uint16_t maxPressureThreshold = 22000;
bool touchCalibrated = true;

uint16_t CAL_TS_LEFT = 934, CAL_TS_RT = 138, CAL_TS_TOP= 840, CAL_TS_BOT= 144;  //Touch screen calibration
uint16_t CAL_TS_WID = 480, CAL_TS_HT = 320; 

uint16_t X_Coord, Y_Coord;      //Note these should be same as tp.x and tp.y temp global for x and y coord of a touch screen press
uint32_t timeLastTouch = 0;
uint32_t lcdTimeoutDuration = 60 * 1000;
bool lcdSleep = false;

_Date nutrientDate = {0,0,0,0};
_Date currentDate = {0,0,0,0};
_Date dateTemp = {0,0,0,0};
_Time sunriseTime = {6,15,0};
_Time sunsetTime = {24,0,0};
_Time sleepTime = {22,30,0};
_Time currentTime = {0,0,0};
_Time newTSTime = {12, 05, 0};
_Time timeTemp = {0,0,0};

uint8_t growWeek;
uint8_t growDay;

uint8_t daylight;
uint8_t night;
int yTime;

//devIs coding:
//0:fan 1:light 2:pump 3:12vaux 4:5vaux 5:redLight HS switch 6:Special 0  
uint8_t devicePins[numDevices] = {0,ACT_LED,ACT_PUMP,ACT_12V,ACT_5V,ACT_HS_AUX,ACT_GND};
int8_t numSlots = 1;
int8_t currentSlot = -1;
int week = 0;
int prevDayOfWeek = 0;                  //saves the previous day of week so we know when we have rolled to a new one.  

//ScheduleBlocks - sets blocks of time where certain devices to be set to a certain state
//1:off, 2:on, 3:auto(default) - device chooses when to be on to meet plants needs, 4:periodic off/on.
//Slots, left to right they are earliest to latest. If the clock finds itself outside a block it is always set to auto
bool devIsAuto[maxNumSlots][numDevices];          //overrides all below component modes is the device on auto mode, 
bool devIsState[maxNumSlots][numDevices];         //manually set what is active in that block - 0th bit = fan 1=light 2=pump...
bool devIsPeriodic[maxNumSlots][numDevices];      //is device in periodic mode for this block, componentIsState must be set to true
uint16_t devPeriodic[maxNumSlots][numDevices][4];      //0th is off duration, 1st is on duration, 2nd is state, 3rd is sinceStateChange
struct _Time startTimeSlots[maxNumSlots];         //the begining of a schedule slot, index zero is default sunrise start, index numSlots -1 is sleep start

bool customApplication = false;                  //set to true to use the technicalNames, and when custom names for devices are needed.
char hydroponicDeviceNames[numDevices][growOSSizeOfDeviceLabel] = {"Fan", "Light", "Pump", "12V", "5V", "Red Light"}; 
char technicalDeviceNames[numDevices][growOSSizeOfDeviceLabel] = {"12V PWM", "Outlet Relay1", "Outlet Relay2", "12V", "5V", "High-Side Switch"}; 
char deviceNames[numDevices][growOSSizeOfDeviceLabel];
char strTemp[_ADA_GFX_sizeOfLabel];
char strTemp2[_ADA_GFX_sizeOfLabel];
char daysSelect[_ADA_GFX_sizeOfLabel]= "All Days";
char weeksSelect[_ADA_GFX_sizeOfLabel] = "All Weeks";
 
byte daysSelected[numDayBtns]  = { 1 };            //0th = All, 1th=Sunday     // 0=OFF 1=ON 2=HIDDEN
byte weeksSelected[numWeekBtns]  = { 1 };          //0th = ALL, 1th = week1 .... 12=week12

Adafruit_GFX_Button dayBtns[numDayBtns];
Adafruit_GFX_Button weekBtns[numWeekBtns];
Adafruit_GFX_Button daysSelectBtn;
Adafruit_GFX_Button weeksSelectBtn;
Adafruit_GFX_Button exitBtn;

//number of seconds for device to be on in auto settings
uint16_t fanQuota = 60*60*20;   //18 hours
uint16_t lightQuota = 60*60*24; //18 hours
uint16_t pumpOffTime = 60;      //time to be off for
uint16_t pumpOnTime = 60;       //time to be on for
uint16_t twelveVQuota = 60*60*16;   //18 hours
uint16_t fiveVQuota = 60*60*16;    //18 hours
uint16_t HS_AUX_STATE = 0;
uint16_t fanSpeed = 40;

bool c = true;                    //show temp in celcius or fahrenheit
float currentTemp = 21;
float maxTempDay = currentTemp;
float minTempDay = currentTemp;
float targetTemp = 21;
float currentHumid = 30;
float maxHumidDay = currentHumid;
float minHumidDay = currentHumid;

int currentPage = 0;          //start at homePage
bool checkTPOnly = false;     //when building a page if checkTPOnly == true, do not rebuild the page only check that page for a relevant touch.
bool updatePageOnly = false;
bool reDrawStuff = false;

bool enBUMODE = false;
bool gotPWROFF_INT = false;
bool enSaveToFlash = false;

char buf[40];