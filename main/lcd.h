#ifndef LCD_H_
#define LCD_H_

#include "globals.h"

void centerTitle(String s, uint16_t textColor, uint16_t bg);
void doubleCenterPrint(String s1, String s2, int len1, int len2, int y, uint16_t textColor, int offset1, int offset2);
void centerPrintCustomFont(String s, int yS, uint16_t textColor, uint16_t bg, int textSize = -1); // By Erik - To clear the previous font that was there
void centerPrintXY(String s, int x, int y, uint16_t textColor, uint16_t bg, int textSize);
void centerPrint(String s, int y, uint16_t textColor, uint16_t bg, int textSize = -1);
void printTextBoxUL(int xPrint, int yPrint, uint16_t outline, uint16_t fill, uint16_t textColor, String s);
int getTextHeight();
int getLongestDeviceNameLabelWidth();
void bofe(char *buf);
char *Aval(int pin);
bool tapWithinUL(int x1, int y1, int w, int h);
bool touchWithinUL(int x1, int y1, int w, int h);
void waitForTap(void);
void touchPage();
void touchHandler(TPoint p, TEvent e);
bool touchIsPressed(void);
void updateGlobalTouchInfo(TPoint p, TEvent e);
void printTouchInfo(TPoint p, TEvent e);
void waitForTouch(int ms);
void getCalibratedPoints(void);
void homePage();
void disablePageLimits();
void lcdWakeup();
void lcdTimeout();
void initializeLCD();
void fail();
void report(void);
void calibrate(int x, int y, int i, String msg);
void calibrateLCD(void);
void calibrateLCDPage(void);

#endif