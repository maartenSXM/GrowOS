#include "globals.h"
#include "lcd.h"
#include "pages.h"

#define SWAP(x, y) \
  {                \
    int t = x;     \
    x = y;         \
    y = t;         \
  }

void initializeLCD()
{
  timeLastTouch = millis();
  tft.begin();
  dimLCDBacklightPercent(40);

  tft.fillScreen(BLACK);
  tft.setRotation(TOUCH_ORIENTATION);
  xDisp = tft.width();
  yDisp = tft.height();
  yCenter = (yDisp / 2) - 1;
  xCenter = (xDisp / 2) - 1;
#ifdef DEBUG
  Serial.println("TFT LCD is initialized. Following vars are set.");
  Serial.println("Touch Orientation = " + String(TOUCH_ORIENTATION));
  Serial.println("xDisp = " + String(xDisp));
  Serial.println("yDisp = " + String(yDisp));
  Serial.println("xCenter = " + String(xCenter));
  Serial.println("yCenter = " + String(yCenter));
#endif
}

// set the lcd brightness
void dimLCDBacklightPercent(uint8_t brightness)
{
  const uint8_t freq = 50;
  const uint8_t ledChannel = 1;
  const uint8_t resolution = 8;
  // Calculate the inverted brightness value (0-255)
  const uint8_t outputBrightness = map(brightness, 0, 100, 255, 0); // maps value (input, fromLow, fromHigh, toLow, toHigh)

  //// attach the channel to the GPIO to be controlled
  // ledcAttachPin(TFT_BACKLIGHT, ledChannel);
  //
  //// configure LED PWM functionalitites
  // ledcSetup(ledChannel, freq, resolution);
  //
  // ledcWrite(ledChannel, 255);

  pinMode(TFT_BACKLIGHT, OUTPUT);
  analogWrite(TFT_BACKLIGHT, outputBrightness); // Brightness
}

void touchHandler(TPoint p, TEvent e)
{
#ifdef DEBUG
  Serial.println(F("** TOUCH **"));
  // TEvent e could be : TEvent::Tap, TEvent::TouchStart, TEvent::TouchMove, TEvent::TouchEnd, TEvent::DragStart, TEvent::DragMove, TEvent::DragEnd,
  printTouchInfo(p, e); // log the coordiantes
  Serial.println(F("** Coordinates above **"));
#endif
  if (lcdSleep)
  {
    lcdSleep = false;
    lcdWakeup();
    return;
  }
  updateGlobalTouchInfo(p, e);
  tp.touched = true;
  touchPage(); // check to see if the touch is on something relevant for
  tp.touched = false;
}

void updateGlobalTouchInfo(TPoint p, TEvent e)
{
  // lets map the touch coords to tp so they are consistent with the display coords. *HACK*
  tp.x = p.y;
  tp.y = yDisp - p.x;
  timeLastTouch = millis();
}

void printTouchInfo(TPoint p, TEvent e)
{
  if (e != TEvent::Tap && e != TEvent::DragStart && e != TEvent::DragMove && e != TEvent::DragEnd)
  {
    Serial.println("Unknown Touch event");
  }

  Serial.print("X: ");
  Serial.print(p.x);
  Serial.print(", Y: ");
  Serial.print(p.y);
  Serial.print(", E: ");

  switch (e)
  {
  // require println to flush the buffer, and avoid overflow
  case TEvent::Tap:
    Serial.println("Tap");
    break;
  case TEvent::DragStart:
    Serial.println("DragStart");
    break;
  case TEvent::DragMove:
    Serial.println("DragMove");
    break;
  case TEvent::DragEnd:
    Serial.println("DragEnd");
    break;
  default:
    Serial.println("UNKNOWN");
    break;
  }
}

void lcdTimeout()
{
  if (lcdSleep)
    return; // already asleep
  uint32_t curTime = millis();
  if (((curTime - timeLastTouch) >= lcdTimeoutDuration) || ((curTime - timeLastTouch) < 0))
  { // the second or case will be less than zero when millis overflows (goes back to zero)
    lcdSleep = true;
    tft.fillScreen(BLACK);
    // setPin(ACT_GND, false);
  }
}

void lcdWakeup()
{
  lcdSleep = false;
  initializeLCD();
  disablePageLimits();
  homePage();
}

void getCalibratedPoints(void)
{
  // Serial.println("tp.x=" + String(tp.x) + ", tp.y=" + String(tp.y) + ", tp.z =" + String(tp.z));
  if (touchCalibrated)
  {
    if (TOUCH_ORIENTATION == PORTRAIT)
    {
      tp.x = map(tp.x, CAL_TS_LEFT, CAL_TS_RT, 0, CAL_TS_HT);
      tp.y = map(tp.y, CAL_TS_TOP, CAL_TS_BOT, 0, CAL_TS_WID);
      // adjust for out of range
      if (tp.x < 0)
        tp.x = 0;
      if (tp.x > CAL_TS_WID)
        tp.x = CAL_TS_WID - 1;
      if (tp.y < 0)
        tp.y = 0;
      if (tp.y > CAL_TS_HT)
        tp.y = CAL_TS_HT - 1;
    }
    else if (TOUCH_ORIENTATION == LANDSCAPE)
    {
      tp.x = map(tp.y, CAL_TS_LEFT, CAL_TS_RT, 0, CAL_TS_WID);
      tp.y = map(tp.x, CAL_TS_TOP, CAL_TS_BOT, 0, CAL_TS_HT);
      // adjust for out of range
      if (tp.x < 0)
        tp.x = 0;
      if (tp.x > CAL_TS_WID)
        tp.x = CAL_TS_WID - 1;
      if (tp.y < 0)
        tp.y = 0;
      if (tp.y > CAL_TS_HT)
        tp.y = CAL_TS_HT - 1;
    }
    else
    {
#ifdef DEBUG
      Serial.println("ERROR: Invalid TOUCH_ORIENTATION");
#endif
    }
  }
}

void waitForTouch(int ms)
{
  int starttime = millis();
  int endtime = starttime;
  bool isTouched = false;
  while (((endtime - starttime) <= ms) && isTouched == false)
  { // do this loop for up to `ms` milliseconds
    if (ts.touched())
    {
      tp.touched = true;
      isTouched = true;
      ts.loop(); // Poll the touch screen controller
      break;
    }
    endtime = millis();
  }
}

void waitForTap(void)
{
  while (ts.touched() == false)
  {
  }
  while (ts.touched() == true)
  {
  }
}

bool touchWithinUL(int x1, int y1, int w, int h)
{
  int x2 = x1 + w, y2 = y1 + h;
  if ((tp.x >= x1) && (tp.x <= x2) && (tp.y >= y1) && (tp.y <= y2))
  {
    return true;
  }
  return false;
}

bool tapWithinUL(int x1, int y1, int w, int h)
{
  int x2 = x1 + w, y2 = y1 + h;
  if ((tp.x >= x1) && (tp.x <= x2) && (tp.y >= y1) && (tp.y <= y2))
  {
    while (ts.touched())
    {
    } // waitForRelease
    return true;
  }
  return false;
}

char *Aval(int pin)
{
  static char buf[2][10];
  uint8_t cnt = 1;
  cnt = !cnt;
#if defined(ESP32)
  sprintf(buf[cnt], "%d", pin);
#else
  sprintf(buf[cnt], "A%d", pin - A0);
#endif
  return buf[cnt];
}

void bofe(char *buf)
{
  tft.println(buf);
  Serial.println(buf);
}

int getLongestDeviceNameLabelWidth()
{
  int16_t x, y;
  uint16_t w, h;
  int longest = -1;

  for (int i = 0; i < numDevices; i++)
  {
    tft.getTextBounds(deviceNames[i], 0, 0, &x, &y, &w, &h);
    if (w > longest)
      longest = w;
  }
  return longest;
}

int getTextHeight()
{
  int16_t x, y;
  uint16_t w, h;
  int tallest = -1;

  for (int i = 0; i < numDevices; i++)
  {
    tft.getTextBounds("tjlgqTJLGQ", 0, 0, &x, &y, &w, &h);
    if (h > tallest)
      tallest = h;
  }
  return tallest;
}

void printTextBoxUL(int xPrint, int yPrint, uint16_t outline, uint16_t fill, uint16_t textColor, String s)
{
  int16_t x, y;
  uint16_t w, h;
  tft.getTextBounds(s, xPrint, yPrint, &x, &y, &w, &h);
  tft.fillRoundRect(x - 5, y - 5, w + 10, h + 10, 10, fill);
  tft.drawRoundRect(x - 5, y - 5, w + 10, h + 10, 10, outline);
  tft.setTextColor(textColor);
  tft.drawString(s, xPrint, yPrint);
}

void centerPrintXY(String s, int x, int y, uint16_t textColor, uint16_t bg, int textSize = -1)
{
  int len;
  if (textSize == -1)
    len = s.length() * 6;
  else
    len = s.length() * textSize;
  tft.setTextColor(textColor, bg);
  tft.drawString(s, x - (len / 2), y);
}

void centerPrintCustomFont(String s, int yS, uint16_t textColor, uint16_t bg, int textSize) // By Erik - To clear the previous font that was there
{
  int len;
  if (textSize == -1)
    len = s.length() * 6;
  else
  {
    tft.setTextSize(textSize);
    len = s.length() * 6 * textSize;
  }

  int16_t xPrint = ((xDisp / 2) - len) - 1;
  int16_t yPrint = yS + 5;
  int16_t x, y;
  uint16_t w, h;

  delay(10);
  tft.getTextBounds(s, xPrint, yPrint, &x, &y, &w, &h);
  delay(10);
  xPrint = xDisp / 2 - w / 2;

  tft.getTextBounds(s, xPrint, yPrint, &x, &y, &w, &h);
  delay(10);
  tft.fillRect(xPrint, y, w + 15, h + 10, bg);

  tft.setTextColor(textColor, bg);
  tft.drawString(s, xPrint + 1, yPrint);
}

void centerPrint(String s, int y, uint16_t textColor, uint16_t bg, int textSize)
{
  int len;
  if (textSize == -1)
    len = s.length() * 6;
  else
  {
    tft.setTextSize(textSize);
    len = s.length() * 6 * textSize;
  }
  tft.setTextColor(textColor, bg);
  tft.drawString(s, ((xDisp - len)) / 2, y - 5);
}

void centerTitle(String s, uint16_t textColor, uint16_t bg)
{
  tft.setFreeFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  // tft.fillRect(0, 0, xDisp, 14, RED);
  tft.fillRect(0, 0, xDisp, 23, WHITE);
  centerPrint(s, 14, textColor, bg);
  // tft.setCursor(0, 30);
  // tft.setTextColor(WHITE, BLACK);
}

void doubleCenterPrint(String s1, String s2, int len1, int len2, int y, uint16_t textColor, int offset1, int offset2)
{
  int middle = xCenter;
  tft.drawString(s1, middle - middle / 2 - len1 / 2 + offset1, y);
  tft.drawString(s2, (middle + middle / 2 - len2 / 2 - offset2), y);
}

void centerNumberPrint(const char *s, int y, uint16_t textColor, uint16_t bg)
{
  int len = strlen(s) * 32;
  tft.setTextColor(textColor);
  tft.setCursor((xDisp - len) / 2, y);
  tft.drawString(s, (xDisp - len) / 2, y);
}

// top left is 0,0 - bottom right is 479,319
void drawCrossHair(int x, int y, uint16_t color) // Draw a cross hair and a box around the given center point
{
  tft.drawRect(x - 9, y - 9, x + 9, y + 9, color); // 10, 150, 20, 20
  Serial.print(x - 9);
  Serial.print(y + 9);
  tft.drawLine(x - 5, y, x + 5, y, color);
  tft.drawLine(x, y - 5, x, y + 5, color);
}

void drawCross(int x, int y, unsigned int color)
{
  tft.drawLine(x - 5, y, x + 5, y, color);
  tft.drawLine(x, y - 5, x, y + 5, color);
}

void showpins(int A, int D, int value, const char *msg)
{
  char buf[40];
  sprintf(buf, "%s (%s, D%d) = %d", msg, Aval(A), D, value);
  Serial.println(buf);
}

// NO diagnosing pins in production board
/*bool diagnosePins()
{
    uint8_t i, j, Apins[2], Dpins[2], found = 0;
    uint16_t value, Values[2];

    Serial.println(F("Making all control and bus pins INPUT_PULLUP"));
    Serial.println(F("Typical 30k Analog pullup with corresponding pin"));
    Serial.println(F("would read low when digital is written LOW"));
    Serial.println(F("e.g. reads ~25 for 300R X direction"));
    Serial.println(F("e.g. reads ~30 for 500R Y direction"));
    Serial.println(F(""));

    for (i = A0; i < A5; i++) pinMode(i, INPUT_PULLUP);
    for (i = 2; i < 10; i++) pinMode(i, INPUT_PULLUP);
    for (i = A0; i < A4; i++) {
        pinMode(i, INPUT_PULLUP);
        for (j = 5; j < 10; j++) {
            pinMode(j, OUTPUT);
            digitalWrite(j, LOW);
            value = analogRead(i);               // ignore first reading
            value = analogRead(i);
            if (value < 100 && value > 0) {
                showpins(i, j, value, "Testing :");
                if (found < 2) {
                    Apins[found] = i;
                    Dpins[found] = j;
                    Values[found] = value;
                }
                found++;
            }
            pinMode(j, INPUT_PULLUP);
        }
        pinMode(i, INPUT_PULLUP);
    }
    if (found == 2) {
        int idx = Values[0] < Values[1];
//                Serial.println(F("Diagnosing as:-"));
//                for (i = 0; i < 2; i++) {
//                    showpins(Apins[i], Dpins[i], Values[i],
//                             (Values[i] < Values[!i]) ? "XM,XP: " : "YP,YM: ");
//                }

        XM = Apins[!idx]; XP = Dpins[!idx]; YP = Apins[idx]; YM = Dpins[idx];
        ts = TouchScreen(XP, YP, XM, YM, 300);    //re-initialise with pins
        return true;                              //success
    }
    if (found == 0) Serial.println(F("MISSING TOUCHSCREEN"));
    //else Serial.println(F("BROKEN TOUCHSCREEN"));
    return false;
}
*/

// bool switchOrientation(uint16_t changeOrientTo){
//    uint16_t tmp;
//
//    if(TOUCH_ORIENTATION == PORTRAIT && changeOrientTo == LANDSCAPE){
//     tmp = CAL_TS_LEFT, CAL_TS_LEFT = CAL_TS_TOP, CAL_TS_TOP = CAL_TS_RT, CAL_TS_RT = CAL_TS_BOT, CAL_TS_BOT = tmp;
//     TOUCH_ORIENTATION == changeOrientTo;
//    }
//    else if(TOUCH_ORIENTATION == PORTRAIT && changeOrientTo == LANDSCAPE){
//     tmp = CAL_TS_LEFT, CAL_TS_LEFT = CAL_TS_TOP, CAL_TS_TOP = CAL_TS_RT, CAL_TS_RT = CAL_TS_BOT, CAL_TS_BOT = tmp;
//     TOUCH_ORIENTATION == changeOrientTo;
//    }
//    //not a valid config, something is broken
//    else{
//     return false;
//    }
//    tft.setRotation(changeOrientTo);
//    xDisp = tft.width();
//    yDisp = tft.height();
//    yCenter = (yDisp / 2) - 1;
//    xCenter = (xDisp / 2) - 1;
//    return true;
// }

// calibrate touch points to be more accurate, this function is unnecsarry as we are already calibrated
void calibrateLCDPage()
{
  calibrateLCD();
}

void calibrateLCD()
{
  touchCalibrated = false;

  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  centerTitle("TouchScreen.h GFX Calibration", BLACK, WHITE);
  tft.println(F("#define NUMSAMPLES 3 in Library\n"));
  tft.println(F("Use a stylus or something"));
  tft.println(F("similar to touch as close"));
  tft.println(F("to the center of the WHITE"));
  tft.println(F("crosshair.  Keep holding"));
  tft.println(F("until crosshair turns RED."));
  tft.println(F("Repeat for all crosshairs.\n"));
  tft.println(F("Report can be pasted from Serial\n"));
  tft.println(F("Touch screen to continue"));

  waitForTap();

  int x, y, cnt, idx = 0;
  tft.fillScreen(BLACK);
  for (x = 10, cnt = 0; x < xDisp; x += (xDisp - 20) / 2)
  {
    for (y = 10; y < yDisp; y += (yDisp - 20) / 2)
    {
      if (++cnt != 5)
        drawCrossHair(x, y, OFFBLACK);
    }
  }
  centerPrint("***********", yCenter - 12, WHITE, BLACK);
  centerPrint("***********", yCenter + 12, WHITE, BLACK);
  for (x = 10, cnt = 0; x < xDisp; x += (xDisp - 20) / 2)
  {
    for (y = 10; y < yDisp; y += (yDisp - 20) / 2)
    {
      if (++cnt != 5)
        calibrate(x, y, idx++, F(" X, Y, Pressure"));
    }
  }

  cals = (long(xDisp - 1) << 12) + (yDisp - 1);
  swapxy = rx[2] - rx[0];
  // else swapxy = ry[2] - ry[0];
  swapxy = (swapxy < -400 || swapxy > 400);
  if (swapxy != 0)
  {
    clx = (ry[0] + ry[1] + ry[2]); // rotate 90
    crx = (ry[5] + ry[6] + ry[7]);
    cty = (rx[0] + rx[3] + rx[5]);
    cby = (rx[2] + rx[4] + rx[7]);
  }
  else
  {
    clx = (rx[0] + rx[1] + rx[2]); // regular
    crx = (rx[5] + rx[6] + rx[7]);
    cty = (ry[0] + ry[3] + ry[5]);
    cby = (ry[2] + ry[4] + ry[7]);
  }
  clx /= 3;
  crx /= 3;
  cty /= 3;
  cby /= 3;
  px = float(crx - clx) / (xDisp - 20);
  py = float(cby - cty) / (yDisp - 20);
  //  px = 0;
  clx -= px * 10;
  crx += px * 10;
  cty -= py * 10;
  cby += py * 10;

  calx = (long(clx) << 14) + long(crx);
  caly = (long(cty) << 14) + long(cby);
  if (swapxy)
    cals |= (1L << 31);

  report(); // report results
}

void readCalibrationCoordinates()
{
  int iter;
  int cnt = 0;
  int failcount = 0;
  uint32_t tx = 0;
  uint32_t ty = 0;
  uint32_t tz = 0;
  bool OK = false;

  while (OK == false)
  {
    centerPrint("*  PRESS  *", yCenter, WHITE, BLACK);
    while (ts.touched() == false)
    {
    }
    centerPrint("*  HOLD!  *", yCenter, WHITE, BLACK);
    cnt = 0;
    iter = 50;
    do
    {
      if (ts.touched()) //.kbv , on first click or wrong click sometimes z is Very high.
      {
        tx += tp.x;
        ty += tp.y;
        tz += tp.z;
        cnt++;
      }
      else
        failcount++;
    } while ((cnt < iter) && (failcount < 10000));
    if (cnt >= iter)
    {
      OK = true;
    }
    else
    {
      tx = 0;
      ty = 0;
      tz = 0;
      cnt = 0;
    }
    if (failcount >= 10000)
      fail();
  }

  cx = tx / iter;
  cy = ty / iter;
  cz = tz / iter;
}

void calibrate(int x, int y, int i, String msg)
{
  drawCrossHair(x, y, WHITE);
  readCalibrationCoordinates();
  centerPrint("* RELEASE *", yCenter, WHITE, BLACK);
  drawCrossHair(x, y, GREEN);
  rx[i] = cx;
  ry[i] = cy;
  char buf[40];
  sprintf(buf, "\r\ncx=%d cy=%d cz=%d %s", cx, cy, cz, msg.c_str());
  Serial.print(buf);
  while (ts.touched() == true)
  {
  }
}

void report()
{
  uint16_t TS_LEFT, TS_RT, TS_TOP, TS_BOT, TS_WID, TS_HT, TS_SWAP;
  int16_t tmp;
  char buf[100];

  tft.fillScreen(BLACK);
  tft.setTextSize(1);
  centerTitle("TouchScreen.h GFX Calibration", BLACK, WHITE);
  // tft.println(F("To use the new calibration"));
  tft.println(F("settings you must map the values"));
  tft.println(F("from Point p = ts.getPoint() e.g. "));
  tft.println(F("x = map(p.x, LEFT, RT, 0, tft.width());"));
  tft.println(F("y = map(p.y, TOP, BOT, 0, tft.height());"));
  tft.println(F("swap p.x and p.y if diff ORIENTATION"));

  //.kbv show human values
  TS_LEFT = (calx >> 14) & 0x3FFF;
  TS_RT = (calx >> 0) & 0x3FFF;
  TS_TOP = (caly >> 14) & 0x3FFF;
  TS_BOT = (caly >> 0) & 0x3FFF;
  TS_WID = ((cals >> 12) & 0x0FFF) + 1;
  TS_HT = ((cals >> 0) & 0x0FFF) + 1;
  TS_SWAP = (cals >> 31);
  if (TOUCH_ORIENTATION == LANDSCAPE)
  { // always show PORTRAIT first
    tmp = TS_LEFT, TS_LEFT = TS_BOT, TS_BOT = TS_RT, TS_RT = TS_TOP, TS_TOP = tmp;
    tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;
  }
  sprintf(buf, "\n\n*** COPY-PASTE from Serial Terminal:");
  bofe(buf);
  bool ts_landscape = (TOUCH_ORIENTATION == LANDSCAPE) ^ swapxy;
#if (USE_XPT2046)
  sprintf(buf, "const int TS_LANDSCAPE=%d; //%s", ts_landscape, "TouchScreen.h GFX Calibration");
  bofe(buf);
#else
  if (ts_landscape)
  {
    SWAP(XM, YP);
    SWAP(XP, YM);
    SWAP(TS_LEFT, TS_RT);
    SWAP(TS_TOP, TS_BOT);
  }
  sprintf(buf, "const int XP=%d,XM=%s,YP=%s,YM=%d; //%dx%d",
          XP, Aval(XM), Aval(YP), YM, TS_WID, TS_HT);
  Serial.println(buf);
  sprintf(buf, "\nTouch Pin Wiring XP=%d XM=%s YP=%s YM=%d",
          XP, Aval(XM), Aval(YP), YM);
  tft.println(buf);
#endif
  sprintf(buf, "const int TS_LEFT=%d,TS_RT=%d,TS_TOP=%d,TS_BOT=%d;",
          TS_LEFT, TS_RT, TS_TOP, TS_BOT);
  Serial.println(buf);

  for (int orient = 0; orient < 2; orient++)
  {
    sprintf(buf, "\n%s CALIBRATION     %d x %d",
            orient ? "LANDSCAPE" : "PORTRAIT ", TS_WID, TS_HT);
    bofe(buf);
    sprintf(buf, "x = map(p.%s, LEFT=%d, RT=%d, 0, %d)",
            orient ? "y" : "x", TS_LEFT, TS_RT, TS_WID);
    bofe(buf);
    sprintf(buf, "y = map(p.%s, TOP=%d, BOT=%d, 0, %d)",
            orient ? "x" : "y", TS_TOP, TS_BOT, TS_HT);
    bofe(buf);
    if (TOUCH_ORIENTATION == orient)
    {
      CAL_TS_LEFT = TS_LEFT;
      CAL_TS_TOP = TS_TOP;
      CAL_TS_RT = TS_RT;
      CAL_TS_BOT = TS_BOT;
      CAL_TS_WID = TS_WID;
      CAL_TS_HT = TS_HT;
    }
    tmp = TS_LEFT, TS_LEFT = TS_TOP, TS_TOP = TS_RT, TS_RT = TS_BOT, TS_BOT = tmp;
    tmp = TS_WID, TS_WID = TS_HT, TS_HT = tmp;
  }

  touchCalibrated = true;

  int16_t x_range = TS_LEFT - TS_RT, y_range = TS_TOP - TS_BOT;
  if (!(abs(x_range) > 500 && abs(y_range) > 650))
  { // LANDSCAPE
    sprintf(buf, "\n*** UNUSUAL CALIBRATION RANGES %d %d", x_range, y_range);
    bofe(buf);
  }
  tft.println("Tap the 'x' in top right corner to exit");
  TFT_eSPI_Button exitButton;
  exitButton.initButton(&tft, 470, 12, 20, 20, OPBOX_GREEN, BLACK, WHITE, "X", 2);
  exitButton.drawButton(false);

  while (!(ts.touched() && exitButton.contains(tp.x, tp.y)))
  {
  }
  return;
}

void fail()
{
  tft.fillScreen(BLACK);
  centerTitle("Touch Calibration FAILED", WHITE, RED);

  tft.println(F("Unable to read the position"));
  tft.println(F("of the press. This is a"));
  tft.println(F("hardware issue and can not"));
  tft.println(F("be corrected in software."));
  tft.println(F("check XP, XM pins with a multimeter"));
  tft.println(F("check YP, YM pins with a multimeter"));
  tft.println(F("should be about 300 ohms"));
}

void testCalibration(void)
{
  tft.fillScreen(BLACK);
  TFT_eSPI_Button exitButton;
  exitButton.initButton(&tft, 470, 10, 20, 20, OPBOX_GREEN, BLACK, WHITE, "X", 2);
  exitButton.drawButton(false);

  // drawCross(30, 30, WHITE);
  // drawCross(tft.width() - 30, tft.height() - 30, WHITE);

  String text;
  char bufX[60];
  char bufY[60];
  text += "Screen rotation = ";
  text += tft.getRotation();
  char buffer[60];
  text.toCharArray(buffer, 30);

  tft.setTextSize(1);
  tft.drawString(buffer, xCenter / 2, 50, 1);
  tft.drawString("Touch anywhere on screen", xCenter / 2, 70, 1);
  tft.drawString("to test settings", xCenter / 2, 90, 1);
  tft.drawString("Tap the 'X' in the top right to exit.", xCenter / 2, 120, 1);

  while (Serial.available())
    Serial.read(); // Empty the serial buffer before we start
  centerPrint("X = ", 5, WHITE, BLACK);
  centerPrint("Y = ", 12, WHITE, BLACK);
  while (true)
  {
    if (ts.touched() == true) // Note this function updates coordinates stored within library variables
    {
      drawCross(tp.x, tp.y, GREEN);

      if (tp.x < 10)
        sprintf(bufX, "X =   %d", tp.x);
      if (tp.x < 100)
        sprintf(bufX, "X =  %d", tp.x);
      if (tp.x >= 100)
        sprintf(bufX, "X = %d", tp.x);
      if (tp.y < 10)
        sprintf(bufY, "Y =   %d", tp.y);
      if (tp.y < 100)
        sprintf(bufY, "Y =  %d", tp.y);
      if (tp.y >= 100)
        sprintf(bufY, "Y = %d", tp.y);
      centerPrint(bufX, 5, WHITE, BLACK);
      centerPrint(bufY, 12, WHITE, BLACK);

      if (exitButton.contains(tp.x, tp.y))
      {
        break;
      }
    }
  }
}
