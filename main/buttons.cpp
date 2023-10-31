#include "buttons.h"

bool awaitingButtonRelease(TFT_eSPI_Button *btn)
{
  if (!btn->contains(tp.x, tp.y))
  { // If this button is not pressed we return false
    return false;
  }
  else
  {                        // else we are waiting for a release and return true to indicate this is the button that is touched.
    btn->drawButton(true); // button is pressed
    while (ts.touched() && btn->contains(tp.x, tp.y))
    {
      ts.loop();            // update last touch coordinate
    }                       // wait for release
    btn->drawButton(false); // draw button as not pressed after release
    return true;
  }
}

bool buttonIsPressed(TFT_eSPI_Button *btn)
{
  if (!btn->contains(tp.x, tp.y))
  { // If this button is not pressed we return false
    return false;
  }
  else
  {
    btn->drawButton(true);
    return true;
  }
}

bool buttonIsHeld(TFT_eSPI_Button *btn, int ms)
{
  int starttime = millis();
  int endtime = starttime;
  bool isHeld = true;
  while (((endtime - starttime) <= ms) && isHeld)
  {            // do this loop for up to ms time
    ts.loop(); // update last touch coordinate
    if (ts.touched() || !btn->contains(tp.x, tp.y))
    {
      isHeld = false;
      break;
    }
    endtime = millis();
  }
  return isHeld;
}

// create and initialize TFT_eSPI_Button objects using static factory methods
void loadGlobalButtons()
{
  // the day of week buttons
  int btnTextSize = 1;
  int xS = 100;
  int w = 135;
  int h = 42;
  int x = xS;
  int y = 95;
  int vSpace = 14;
  int hSpace = 10;
  dayBtns[0].initButtonUL(&tft, (xCenter - (w / 2)), y - (h + vSpace), w, h, onOutline, onFill, textColour, "All Days", btnTextSize);
  for (int i = 1; i < numDayBtns; i++)
  {
    if (i == 1)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Sunday", btnTextSize);
    if (i == 2)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Monday", btnTextSize);
    if (i == 3)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Tuesday", btnTextSize);
    if (i == 4)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Wednesday", btnTextSize);
    if (i == 5)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Thursday", btnTextSize);
    if (i == 6)
      dayBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, "Friday", btnTextSize);
    if (i == 7)
      dayBtns[i].initButtonUL(&tft, (xCenter - (w / 2)), y, w, h, onOutline, onFill, textColour, "Saturday", btnTextSize);
    x += w + hSpace;
    if (x + w + hSpace > xDisp)
    {
      x = xS;
      y += (h + vSpace);
    }
  }

  // the week buttons
  btnTextSize = 1;
  xS = 35;
  w = 95;
  h = 50;
  x = xS;
  y = 90;
  vSpace = 10;
  hSpace = 10;
  weekBtns[0].initButtonUL(&tft, (xCenter - (w / 2)), y - (h + vSpace), w, h, onOutline, onFill, textColour, "All Weeks", btnTextSize);
  for (int i = 1; i < numWeekBtns; i++)
  {
    strcpy(strTemp, "W ");
    int_to_char_cat(strTemp, i);
    weekBtns[i].initButtonUL(&tft, x, y, w, h, onOutline, onFill, textColour, strTemp, btnTextSize);
    x += w + hSpace;
    if (x + w + hSpace > xDisp)
    {
      x = xS;
      y += (h + vSpace);
    }
  }

  weeksSelectBtn.initButtonUL(&tft, 1, 30, 160, 40, GREY, BLACK, WHITE, weeksSelect, btnTextSize);
  daysSelectBtn.initButtonUL(&tft, 1, 75, 160, 40, GREY, BLACK, WHITE, daysSelect, btnTextSize);
  exitBtn.initButton(&tft, xDisp - 25, 26, 46, 46, OPBOX_GREEN, BLACK, WHITE, " ", btnTextSize);
}

void updateDaysSelectedBtnLabel()
{
  if (daysSelected[0] == 1)
  {
    strcpy(daysSelect, "All Days");
    daysSelectBtn.setLabel(daysSelect);
    return;
  }
  bool allSelected = true;
  for (int i = 1; i < numDayBtns; i++)
  {
    if (daysSelected[i] == 0)
    {
      allSelected = false;
    }
  }
  if (allSelected)
  {
    strcpy(daysSelect, "All Days");
    daysSelectBtn.setLabel(daysSelect);
    return;
  }
  else
  {
    strcpy(daysSelect, "Days(");
    for (int i = 1; i < numDayBtns; i++)
    { // all buttons but the "All days button"
      if (strlen(daysSelect) >= (maxLabelLength - 6))
      { // dont append to label if getting close to max size of label
        strcat(daysSelect, "...");
        break;
      }
      if ((i == 1) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "Su,");
        continue;
      }
      if ((i == 2) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "M,");
        continue;
      }
      if ((i == 3) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "T,");
        continue;
      }
      if ((i == 4) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "W,");
        continue;
      }
      if ((i == 5) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "Th,");
        continue;
      }
      if ((i == 6) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "F,");
        continue;
      }
      if ((i == 7) && (daysSelected[i] == 1))
      {
        strcat(daysSelect, "Sa)");
        continue;
      }
      else if (i == 7)
        daysSelect[strlen(daysSelect) - 1] = ')';
    }
  }
  daysSelectBtn.setLabel(daysSelect);
}

void updateWeeksSelectedBtnLabel()
{
  if (weeksSelected[0] == 1)
  {
    strcpy(weeksSelect, "AllWeeks");
    weeksSelectBtn.setLabel(weeksSelect);
    return;
  }
  bool allSelected = true;
  for (int i = 1; i < numWeekBtns; i++)
  {
    if (weeksSelected[i] == 0)
    {
      allSelected = false;
    }
  }
  if (allSelected)
  {
    strcpy(weeksSelect, "AllWeeks");
    weeksSelectBtn.setLabel(weeksSelect);
    return;
  }
  else
  {
    strcpy(weeksSelect, "Weeks(");
    int streak = false;
    for (int i = 1; i < numWeekBtns; i++)
    { // start at 1 becuase 0th is ALL selected, dont do last button becuase special case to close off the streak
      if (strlen(weeksSelect) >= (maxLabelLength - 4))
      { // dont append to label if getting close to max size of label
        strcat(weeksSelect, "...");
        break;
      }
      if (i < (numWeekBtns - 1))
      { // all buttons but last
        if (!streak && (weeksSelected[i] == 1) && (weeksSelected[i + 1] == 0))
        {
          int_to_char_cat(weeksSelect, i);
          strcat(weeksSelect, ",");
          i++;
        }
        else if (!streak && (weeksSelected[i] == 1) && (weeksSelected[i + 1] == 1))
        {
          streak = true;
          int_to_char_cat(weeksSelect, i);
          strcat(weeksSelect, "-");
        }
        else if (streak && weeksSelected[i] == 0)
        {
          streak = false;
          int_to_char_cat(weeksSelect, i - 1);
          strcat(weeksSelect, ",");
        }
        else
        {
        } // streak and week is set, or its not a streak and not set
      }
      else
      { // last button
        if (weeksSelected[i] == 1)
        {
          int_to_char_cat(weeksSelect, i);
          strcat(weeksSelect, ")");
        }
        else if (streak && weeksSelected[i] == 0)
        {
          int_to_char_cat(weeksSelect, i - 1);
          strcat(weeksSelect, ")");
        }
        else
        {
          weeksSelect[strlen(weeksSelect) - 1] = ')';
        }
      }
    }
  }
  weeksSelectBtn.setLabel(weeksSelect);
}
