/*With few exceptions pages should not hold the processor here. They should build the current page,
Then set the currentPage variable to the number for the page it just drew.

When a touch occurs we go to the respective currentPage and check if the tp.x and tp.y coords
match to a button on that page. We handle what to do for that button or touch in the code here

*/

// this function is called when there is a touch on the lcd coming from the main loop
// We next need to route to the currentPage and see if the touch tp.x and tp.y
// corresponds to a button on that page

#include "pages.h"

void touchPage()
{
  disablePageLimits();
  checkTPOnly = true;
  pageRouter();
  checkTPOnly = false;
}

void updatePage()
{
  disablePageLimits();
  updatePageOnly = true;
  pageRouter();
  updatePageOnly = false;
}

void reDrawPage()
{
  disablePageLimits();
  reDrawStuff = true;
  pageRouter();
  reDrawStuff = false;
}

// this may need to be called if calling a new page that needs to be built from fresh
// from within a page that was called with any of the below limits set.
void disablePageLimits()
{
  updatePageOnly = false;
  checkTPOnly = false;
  reDrawStuff = false;
}

// called when we need to go to the current page
void pageRouter()
{
  switch (currentPage)
  {
  case 0:
    homePage();
    break;
  case 1:
    settingPage();
    break;
  case 2:
    schedulePage();
    break;
  case 3:
    daysSelectPage();
    break;
  case 4:
    weeksSelectPage();
    break;
  case 5:
    newTimeslotPage();
    break;
  case 6:
    maxTimeSlotsPage();
    break;
  case 7:
    splashPage();
    break;
  case 8:
    deviceInfoPage();
    break;
  case 9:
    initialWifiSetupPage();
    // unknown currentPage value.... build home screen
    break;
  default:
    currentPage = 0;
    homePage();
    break;
  }
}

void homePage()
{
  currentPage = 0;
  // only checking if a touch is corresponding to a button
  if (checkTPOnly)
  {
    // setting Button
    if (tp.x > 440 && tp.y < 75)
    {
      disablePageLimits(); // disable page limits that were used for this page but no longer valid for building next page from fresh
      settingPage();       // build settingPage
    }
    return;
  }

  // If we are building page from scratch clear screen
  if (!updatePageOnly)
  {
    // Clear screen to remove previous artifacts
    tft.fillScreen(DARKGREY);
  }
  // set or update rest of page

  pollCurrentTime();
  float daylight = countDeviceOnTime(1); // seconds on for light
  float night = secondsInDay - daylight;
  float sunriseF = 0;
  for (int slot = 0; slot < numSlots; slot++)
  {
    if (devIsAuto[slot][1] == 1 || devIsState[slot][1] == 1)
    { // find the first slot the light is on, that is when sunRise is
      sunriseF = secondsSinceNewDay(startTimeSlots[slot]);
      break;
    }
  }
  // Serial.println("since sun rise " + String(sunriseTime));

  // assume sunsetTime is the start of the last time slot
  float sunsetTime = secondsSinceNewDay(startTimeSlots[numSlots - 1]);
  float sunset = sunsetTime - secondsSinceNewDay(); // Time until sunset default of function is the currentTime
  float sunshine = secondsSinceNewDay() - sunriseF; // elapsed sunshine time so far

  int middle = xDisp / 2;

  int textColor = WHITE;
  // Serial.println("SinceNewDay " + String(secondsSinceNewDay()));  //ok
  // Serial.println("sunshine " + String(sunshine));
  Serial.println("daylight " + String(daylight));
  float sunshineRatio = sunshine / daylight; // Percentage of time sun has been up for
  float sunbarWidth = 0.6;                   // Percentage of screen width taken up by sun bar
  int sunbarMargin = 15;                     // Margin around sunbar

  float sunbarX = ((xDisp * (1 - sunbarWidth) / 2) + (xDisp * sunbarWidth * sunshineRatio));
  float sunbarY = yDisp / 4;

  String sfanSpeed = String(fanSpeed);

  String stemp = String(currentTemp);
  String shumidity = String(currentHumid);
  String sweek = String(week);
  String sday = String(currentDate.day);

  int tempLength = stemp.length() * 32;
  int humidityLength = shumidity.length() * 32;

  String smaxTemp = String(maxTempDay);
  String sminTemp = String(minTempDay);
  String smaxHumidity = String(maxHumidDay);
  String sminHumidity = String(minHumidDay);

  int lenMaxTemp = (5 + smaxTemp.length()) * 6;
  int lenMinTemp = (5 + sminTemp.length()) * 6;
  int lenMaxHumidity = (5 + smaxHumidity.length()) * 6;
  int lenMinHumidity = (5 + sminHumidity.length()) * 6;

  int tempHumidityY = yDisp / 2.7;

  String stempUnit = "C";
  if (c == false)
  {
    stempUnit = "F";
  }
  // settings button
  tft.drawBitmap8Scale(465, 40, setting50x50, 50, 50, GREY, 0.5);

  // Fill the sunbar based on set width and time of day
  tft.fillRect((xDisp * ((1 - sunbarWidth) / 2)), sunbarY - 30, ((xDisp * ((1 - sunbarWidth) / 2)) + (xDisp * sunbarWidth)), sunbarY, textColor);
  tft.fillRect((xDisp * ((1 - sunbarWidth) / 2)), sunbarY - 30, sunbarX, sunbarY, YELLOW);

  // Display time till sunset
  tft.setFreeFont(NULL);
  tft.setCursor((((xDisp * ((1 - sunbarWidth) / 2)) + (xDisp * sunbarWidth)) + sunbarMargin), sunbarY - 20);
  tft.setTextSize(1);
  tft.setTextColor(YELLOW);
  int sunsetH = (int)(sunset) / 3600;
  int sunsetM = ((int)(sunset) % 3600) / 60; // how many seconds left over after the hours modulo, then divide that by 60 to get seconds
  tft.println(String(sunsetH) + ":" + String(sunsetM));

  tft.setFreeFont(&FreeSevenSegNumFont);
  tft.setTextColor(textColor);

  // Display stemperature, humidity, max/min stemp, max/min humidity
  // Responsive based on single or double digit numbers - triple digit and negative numbers not yet supported

  // NEEDS TO BE EDITED FOR NEGATIVE stempERATURE POSSIBILITY & 100% HUMIDITY POSSIBILITY
  // FOR BOTH OR JUST 1 OF THESE CASES

  if (stemp.length() > 1 && stemp.length() < 3 && shumidity.length() > 1 && shumidity.length() < 3)
  {
    doubleCenterPrint(stemp, shumidity, tempLength, humidityLength, tempHumidityY, textColor, tempLength / 2, humidityLength / 2);
    tft.setFreeFont(NULL);
    tft.drawString("o", middle - middle / 2 + tempLength, tempHumidityY - 4);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.drawString(stempUnit, middle - middle / 2 + tempLength + 5, tempHumidityY);
    tft.drawString("%", middle + middle / 2, tempHumidityY);
    tft.setFreeFont(NULL);
    doubleCenterPrint("Max: " + smaxTemp, "Max: " + smaxHumidity, tempLength, humidityLength, tempHumidityY * 1.7, textColor, tempLength - lenMaxTemp / 2, lenMaxHumidity / 2);
    doubleCenterPrint("Min: " + sminTemp, "Min: " + sminHumidity, tempLength, humidityLength, tempHumidityY * 1.7 - 12, textColor, tempLength - lenMinTemp / 2, lenMinHumidity / 2);
  }
  else if (stemp.length() == 1 && shumidity.length() > 1 && shumidity.length() < 3)
  {
    doubleCenterPrint(stemp, shumidity, tempLength, humidityLength, tempHumidityY, textColor, tempLength, humidityLength / 2);
    tft.setFreeFont(NULL);
    tft.drawString("o", middle - middle / 2 + tempLength + tempLength / 2, tempHumidityY - 4);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.drawString(stempUnit, middle - middle / 2 + tempLength + 5 + tempLength / 2, tempHumidityY);
    tft.drawString("%", middle + middle / 2, tempHumidityY);
    tft.setFreeFont(NULL);
    doubleCenterPrint("Max: " + smaxTemp, "Max: " + smaxHumidity, tempLength, humidityLength, tempHumidityY * 1.7, textColor, tempLength - lenMaxTemp / 2 + tempLength / 2, lenMaxHumidity / 2);
    doubleCenterPrint("Min: " + sminTemp, "Min: " + sminHumidity, tempLength, humidityLength, tempHumidityY * 1.7 - 12, textColor, tempLength - lenMinTemp / 2 + tempLength / 2, lenMinHumidity / 2);
  }
  else if (shumidity.length() == 1 && stemp.length() > 1 && stemp.length() < 3)
  {
    doubleCenterPrint(stemp, shumidity, tempLength, humidityLength, tempHumidityY, textColor, tempLength / 2, humidityLength);
    tft.setFreeFont(NULL);
    tft.drawString("o", middle - middle / 2 + tempLength, tempHumidityY - 4);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.drawString(stempUnit, middle - middle / 2 + tempLength + 5, tempHumidityY);
    tft.drawString("%", middle + middle / 2 - humidityLength / 2, tempHumidityY);
    tft.setFreeFont(NULL);
    doubleCenterPrint("Max: " + smaxTemp, "Max: " + smaxHumidity, tempLength, humidityLength, tempHumidityY * 1.7, textColor, tempLength - lenMaxTemp / 2, lenMaxHumidity / 2 + humidityLength / 2);
    doubleCenterPrint("Min: " + sminTemp, "Min: " + sminHumidity, tempLength, humidityLength, tempHumidityY * 1.7 - 12, textColor, tempLength - lenMinTemp / 2, lenMinHumidity / 2 + humidityLength / 2);
  }
  else if (shumidity.length() == 1 && shumidity.length() == 1)
  {
    doubleCenterPrint(stemp, shumidity, tempLength, humidityLength, tempHumidityY, textColor, tempLength, humidityLength);
    tft.setFreeFont(NULL);
    tft.drawString("o", middle - middle / 2 + tempLength + tempLength / 2, tempHumidityY - 4);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.drawString(stempUnit, middle - middle / 2 + tempLength + 5 + tempLength / 2, tempHumidityY);
    tft.drawString("%", middle + middle / 2 - humidityLength / 2, tempHumidityY);
    tft.setFreeFont(NULL);
    doubleCenterPrint("Max: " + smaxTemp, "Max: " + smaxHumidity, tempLength / 2, humidityLength / 2, tempHumidityY * 1.7, textColor, tempLength - lenMaxTemp / 2 + tempLength / 2, lenMaxHumidity / 2 + humidityLength / 2);
    doubleCenterPrint("Min: " + sminTemp, "Min: " + sminHumidity, tempLength / 2, humidityLength / 2, tempHumidityY * 1.7 - 12, textColor, tempLength - lenMinTemp / 2 + tempLength / 2, lenMinHumidity / 2 + humidityLength / 2);
  }
  else
  {
    tft.setFreeFont(NULL);
    tft.drawString("Humidity: Err 420", middle + middle / 6, tempHumidityY);
    tft.drawString("Temp: Err 420", middle - middle / 2, tempHumidityY);
  }

  // Display speed of fan
  tft.setFreeFont(&FreeSans12pt7b);
  centerPrint("Fan: " + String(fanSpeed) + "%", yDisp * 0.75, textColor, BLACK, 12);

  // Display week and day of grow
  tft.setFreeFont(&FreeSans9pt7b);
  int lenWeek = (6 * 9);
  int lenDay = (6 * 9);

  if (sweek.length() > 1)
  {
    int lenWeek = (7 * 9);
    int lenDay = (6 * 9);
  }
  doubleCenterPrint("Week " + String(week), "Day " + String(currentDate.day), lenWeek, lenDay, yDisp * 0.9, textColor, 6 * 9, 6 * 9);
}

void settingPage()
{
  currentPage = 1;

  if (checkTPOnly)
  {
    if (awaitingButtonRelease(&exitBtn))
    {
      disablePageLimits(); // disable page limits that were used for this page but no longer valid for building next page from fresh
      homePage();          // draw a fresh homepage
      return;
    }
    return;
  }
  if (!updatePageOnly)
  {
    // Clear screen and set title
    tft.fillScreen(BLACK);
    tft.setTextSize(1);
    centerTitle("Settings", BLACK, WHITE);
    exitBtn.drawButton(false);
    tft.drawBitmap(430, 10, home50x50, 50, 50, OPBOX_GREEN, 1); // home button icon top right
  }
  // below is what gets updated on the setting page every loop repetition
}

void deviceInfoPage()
{
  uint8_t yHeight = 20;
  uint8_t derived_mac_addr[6] = {0};
  char buff[55];

  sprintf(buff, "base MAC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", ESP32_BASE_MAC_ADDRESS[0], ESP32_BASE_MAC_ADDRESS[1], ESP32_BASE_MAC_ADDRESS[2],
          ESP32_BASE_MAC_ADDRESS[3], ESP32_BASE_MAC_ADDRESS[4], ESP32_BASE_MAC_ADDRESS[5]);
  centerPrintCustomFont(buff, yHeight, WHITE, BLACK, 2);

  yHeight += 20;
  strcpy(buff, "");
  esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA);
  sprintf(buff, "WIFI_STA MAC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
          derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
  centerPrintCustomFont(buff, yHeight, WHITE, BLACK, 2);

  yHeight += 20;
  strcpy(buff, "");
  esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_SOFTAP);
  sprintf(buff, "WIFI_STA MAC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
          derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
  centerPrintCustomFont(buff, yHeight, WHITE, BLACK, 2);

  yHeight += 20;
  strcpy(buff, "");
  esp_read_mac(derived_mac_addr, ESP_MAC_BT);
  sprintf(buff, "WIFI_STA MAC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
          derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
  centerPrintCustomFont(buff, yHeight, WHITE, BLACK, 2);

  yHeight += 20;
  strcpy(buff, "");
  esp_read_mac(derived_mac_addr, ESP_MAC_ETH);
  sprintf(buff, "WIFI_STA MAC: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
          derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);
  centerPrintCustomFont(buff, yHeight, WHITE, BLACK, 2);
}

// This is a special page, it does not follow the templatePage()
void splashPage()
{
  currentPage = 7;
  /*
  header file/prototype for draw functions
  drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,int16_t h, uint16_t color),
  drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w,int16_t h, int16_t bg = 0x0000),
  tft.drawRGBBitmap(20, 20, tractor10_96x64, 96, 64);      //for 16bit colour bitmaps. draws from top left of iamge
  tft.drawBitmap(50, 50, tractor_128x64, 128, 64, BLUE);  // for 8bit monochrome bit map works. draws from top left of image

  //tft.drawBitmap(50, 50, sun50x50, 50, 50, BLUE);  // for 8bit monochrome bit map works. draws from top left of image
  //tft.drawBitmap(200, 200, sun50x50, 50, 50, GREEN, 2.4);
  */
  ButtonWidget enterButton = ButtonWidget(&tft);

  if (checkTPOnly)
  { // only checking for updating when a button is pressed on the page
    if (ts.touched() && enterButton.contains(tp.x, tp.y))
    {
      enterButton.drawButton(true);
      // wait for release
      while (ts.touched() && enterButton.contains(tp.x, tp.y))
      {
      }
      // draw button as not pressed after release
      enterButton.drawButton(false);
    }
    return;
  }

  if (!updatePageOnly && !reDrawStuff)
  {                        // only runs if we are building page from scratch, enters here if you call the page function directly without going through updatePage()
    tft.fillScreen(BLACK); // Clear screen to remove previous artifacts
    tft.drawRGBBitmap(xCenter - 240 / 2, yCenter - 110 / 2, opbox_logo_white_blackBG240x110, 240, 110, BLACK);
    reDrawStuff = true;
  }

  if (reDrawStuff)
  {
    // Draw button and set as not pressed
    tft.setFreeFont(NULL);
    enterButton.initButton(xCenter, yCenter + 60, 100, 40, OPBOX_GREEN, BLACK, WHITE, "ENTER", 2);
    enterButton.drawButton(false);

  } // below sets/updates rest of page every time the main loop repeats
}

void initialWifiSetupPage(int n)
{
  if (checkTPOnly)
  { // only checking for updating when a button is pressed on the page

    return;
  }

  if (!updatePageOnly && !reDrawStuff)
  {                        // only runs if we are building page from scratch, enters here if you call the page function directly without going through updatePage()
    tft.fillScreen(BLACK); // Clear screen to remove previous artifacts

    reDrawStuff = true;
  }

  if (reDrawStuff)
  {

  } // below sets/updates rest of page every time the main loop repeats
}

// THIS IS AN EXAMPLE FUNCTION
void templatePage()
{
  currentPage = 5;

  if (checkTPOnly)
  { // only checking for updating when a button is pressed on the page

    return;
  }

  if (!updatePageOnly && !reDrawStuff)
  {                        // only runs if we are building page from scratch, enters here if you call the page function directly without going through updatePage()
    tft.fillScreen(BLACK); // Clear screen to remove previous artifacts

    reDrawStuff = true;
  }

  if (reDrawStuff)
  {

  } // below sets/updates rest of page every time the main loop repeats
}
