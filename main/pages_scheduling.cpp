#include "pages_scheduling.h"

void drawScheduleDevices(int slot, int x, int y, int slotWidth){

  int cx,cy;
  int tx, ty;
  int radius;
  int yDiv;
  
  if(customApplication){
    tft.setFont(NULL);
    tft.setTextSize(1);
    uint8_t textSize  = 1;
    cx = 9;
    cy = 4;
    tx = cx*2;
    ty = 0;
    radius = 3;
    yDiv = 4;
    y += 2;
    slotWidth -= 10;
  }
  else{
    tft.setFont(&FreeSans9pt7b);
    tft.setTextSize(1);
    uint8_t textSize  = 1;
    cx = 8;
    cy = 4;
    tx = cx*2 + 8;
    ty = 1;
    radius = 5;
    yDiv = 6;
    y += 11;
    slotWidth -= 10;
  }

  int longestLabelPx = getLongestDeviceNameLabelWidth();
  int labelHeight = getTextHeight();
  int textAndCircle = tx+longestLabelPx;
  
  for(int i=0; i<numDevices; i++){              //if auto draw...
    strcpy(strTemp2, "");
    getDeviceName(strTemp2, i);
    if(devIsAuto[slot][i]){
      if(slotWidth > (radius*2 + 2))tft.drawCircle(x+cx,y+cy,radius,YELLOW);
      if(slotWidth > textAndCircle)tft.print(strTemp2,x+tx,y+ty);
    }
    else if(devIsPeriodic[slot][i]){            //if periodic draw...
      if(slotWidth > (radius*2 + 2))tft.drawCircle(x+cx,y+cy,radius,GREEN);
      if(slotWidth > textAndCircle)tft.print(strTemp2,x+tx,y+ty);
    }       
    else{                                       //else draw based on the manual setting
      if(devIsState[slot][i]){
        if(slotWidth > (radius*2 + 2))tft.fillCircle(x+cx,y+cy,radius,GREEN);
        if(slotWidth > textAndCircle)tft.print(strTemp2,x+tx,y+ty);     
      }
      else{
        if(slotWidth > (radius*2 + 2))tft.drawCircle(x+cx,y+cy,radius,DARKGREY);
        if(slotWidth > textAndCircle)tft.print(strTemp2,x+tx,y+ty);   
      }
    }
    y += (labelHeight + yDiv);                       //When advancing to a new line it is textSize * 8
  }
}

void schedulePage(){
  currentPage = 2;

  Adafruit_GFX_Button newTSBtn;
  newTSBtn.initButtonUL(&tft, xCenter-45, yCenter-72, 100, 30, GREY, BLACK, WHITE, "+ Timeslot", 1);

  int timeSlotColor = CYAN;
  
  int xMinTL = 10;                                                        //foundational margin dimensions for timeline bar, and timeslot
  int xMaxTL = xDisp-10;
  int yMinTL = yDisp-17;
  int yMaxTL = yCenter-20;

  if(checkTPOnly){ 
    if(awaitingButtonRelease(&exitBtn)){
      disablePageLimits();                                                //disable page limits that were used for this page but no longer valid for building next page from fresh
      homePage();                                                         //draw a fresh
    }
    if(awaitingButtonRelease(&daysSelectBtn)){
      disablePageLimits();                                                //disable page limits that were used for this page but no longer valid for building next page from fresh
      daysSelectPage();                                                   //draw a fresh 
    }
    if(awaitingButtonRelease(&weeksSelectBtn)){
      disablePageLimits();                                                //disable page limits that were used for this page but no longer valid for building next page from fresh
      weeksSelectPage();                                                   //draw a fresh
    }
    if(awaitingButtonRelease(&newTSBtn)){
      disablePageLimits();                                                //disable page limits that were used for this page but no longer valid for building next page from fresh
      newTimeslotPage();                                                  //draw a fresh
    }
    
    return;
  }

  //initial build
  if(!updatePageOnly){
    //Clear screen and set title
    tft.fillScreen(BLACK);
    tft.setTextSize(1);
    centerTitle("Scheduling", BLACK, BLACK);
    //tft.fillRect(30,0,400,60,RED);                    //TEST
    
    updateDaysSelectedBtnLabel();
    updateWeeksSelectedBtnLabel();

    tft.setFont(&FreeSans9pt7b);
    daysSelectBtn.drawButton(false);
    weeksSelectBtn.drawButton(false);
    exitBtn.drawButton(false);
    tft.drawBitmap8(430, 10, home50x50, 50, 50, OPBOX_GREEN, 1);     //home button icon top right
    newTSBtn.drawButton(false);

    //tft.fillRect(xMinTL, yMaxTL, xMaxTL, yMinTL, WHITE);
    tft.drawLine(xMinTL, yMaxTL, xMaxTL, yMaxTL, GREY);
    tft.drawLine(xMinTL, yMinTL, xMaxTL, yMinTL, GREY);

    int color = 255;
    int inSet = -30;
    for(int a = inSet ;a <= xMinTL; a++){
      int grad = map(a,inSet,xMinTL,128,255);                                                                 //colour map - maps value (currentData, fromLow, fromHigh, toLow, toHigh)t
      tft.drawLine(xMinTL+(a), yMinTL-1, xMinTL+(a), yMaxTL+1, tft.color565(255-grad, 255-grad, 255-grad));   //Left most gradient
      tft.drawLine(xMaxTL-(a), yMinTL-1, xMaxTL-(a), yMaxTL+1, tft.color565(255-grad, 255-grad, 255-grad));   //Right most gradient
    }
    
    for(int i = 0; i<numSlots; i++){                                                                          //The timeSlot bars vertical line, time stamps, and schedule details
      int seconds = secondsSinceNewDay(startTimeSlots[i]);
      int x = map(seconds,0,secondsInDay,xMinTL,xMaxTL);                                                      //x slot map - maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
      int rectLeft = x -19;
      tft.drawLine(x, yMinTL+5, x, yMaxTL-5, timeSlotColor);
      tft.fillRectUL(rectLeft, yMinTL+5, 38, 12, BLACK);
      tft.drawRectUL(rectLeft, yMinTL+5, 38, 12, timeSlotColor);
      tft.setFont(NULL);
      tft.setTextSize(0.5);
      sprintf(strTemp, "%02d:%02d", startTimeSlots[i].hour, startTimeSlots[i].minute);
      tft.print(strTemp, rectLeft+5, yMinTL+7 );
      
      if(numSlots == 1){                                                                //the first one and only
        if(x <= (xDisp/2)) drawScheduleDevices(i, x, yMaxTL, xDisp/2);                  //if slot mostly on left, draw right of slot
        else drawScheduleDevices(i, xMinTL, yMaxTL, xDisp/2);                           //if slot mostly on right, draw on far left
      }
      else if(i < (numSlots-1)){                                                        //the middle ones
        int diffToNext = map(secondsSinceNewDay(startTimeSlots[i+1]),0,secondsInDay,xMinTL,xMaxTL) - x;
        drawScheduleDevices(i, x, yMaxTL, diffToNext);
      }
      else if(i == (numSlots-1)){                                                       //the last
        int diffToEnd = xMaxTL - x;
        int startToFirst = map(secondsSinceNewDay(startTimeSlots[0]),0,secondsInDay,xMinTL,xMaxTL) - xMinTL;
        if(diffToEnd > startToFirst) drawScheduleDevices(i, x, yMaxTL, diffToEnd); //the space at the end of the schedule is larger than the start to first
        else drawScheduleDevices(i, xMinTL, yMaxTL, startToFirst);
      }
    }
    tft.setFont(&FreeSans9pt7b);
  }

  if((daysSelected[0] == 1) || (weeksSelected[0] == 1) || (weeksSelected[week] && daysSelected[currentDate.dayOfWeek + 1])){
    int x = map(secondsSinceNewDay(),0,secondsInDay,xMinTL,xMaxTL);       //The current time - seconds since new day
    int yL = yMaxTL+5;
    int yH = yMaxTL-14;
    tft.drawLine(x, yL, x, yH, GREEN);
    int msAnimation = 400;
    for(int i = 0 ; i<msAnimation; i++){                                           //Fade to black animation
      int col = map(i,0,msAnimation,0,255);                                        //maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
      tft.drawLine(x, yL, x, yH, tft.color565(0, 255-col, 0));
      delay(1);
    }
    tft.drawLine(x-1, yL, x-1, yH, BLACK);                                          //to fix moving flashing green bar.
    tft.drawLine(x+1, yL, x+1, yH, BLACK);  
    tft.drawPixel(x-1, yMaxTL, GREY);
    tft.drawPixel(x+1, yMaxTL, GREY);
    tft.drawLine(x, yL, x, yH, GREEN);
  }

}

void daysSelectPage(){
  currentPage = 3;
  
  int i,j;
  
  for(i = 0; i<numDayBtns; i++){                                              //Set button colours based on if ON or OFF or HIDDEN
    if((i == 0) && (daysSelected[0] == 1)){             
      dayBtns[i].setFill(onFill);
      dayBtns[i].setOutline(onOutline);
      for(j = 1; j<numDayBtns; j++){
        daysSelected[j] = 2;                                                  //hidden
        dayBtns[j].setFill(offFill);
        dayBtns[j].setOutline(offOutline);
        dayBtns[j].setTextcolor(hiddenTextcolor);
      }
      break;
    }
    else if(daysSelected[i] != 1){
      dayBtns[i].setFill(offFill);
      dayBtns[i].setOutline(offOutline);
      dayBtns[i].setTextcolor(WHITE);
    }
    else if(daysSelected[i] == 1){
      dayBtns[i].setFill(onFill);
      dayBtns[i].setOutline(onOutline);
      dayBtns[i].setTextcolor(WHITE);
    }
  }

  if(checkTPOnly){ 
    if(awaitingButtonRelease(&exitBtn)){
      disablePageLimits();                                                //disable page limits that were used for this page but no longer valid for building next page from fresh
      schedulePage();                                                     //draw a fresh homepage
      return;
    }
    for(i = 0; i<numDayBtns; i++){
      if((i == 0) && awaitingButtonRelease(&dayBtns[i])){                         //Look at the All button first
        if(daysSelected[0] == 0){
          daysSelected[0] = 1;                                                //All days are now selected
          for(j = 1; j<numDayBtns; j++) daysSelected[j] = 2;
        }
        else if(daysSelected[0] == 1){
          daysSelected[0] = 0;                                                ////All days are now de-selected
          for(j = 1; j<numDayBtns; j++) daysSelected[j] = 1;
        }
        reDrawPage();
        return;
      }
      else if(daysSelected[1] != 2){                                          //Now lets look at the lower res buttons that are not hidden
        int numDaysSelected = 0;
        int lastDay;
        for(j = 1; j<numDayBtns; j++){
          if(daysSelected[j] == 1){
            numDaysSelected++;
            lastDay = j;
          }
        }
        if(numDaysSelected<1){                                                  //should never happen
          daysSelected[0] = 1;
          reDrawPage();
          return;
        }
        if((numDaysSelected == 1) && dayBtns[lastDay].contains(tp.x,tp.y)){     //Dont disable the last remaining day button to be turned off
          return;
        }
        if(awaitingButtonRelease(&dayBtns[i])){
          daysSelected[i] = !daysSelected[i];
          reDrawPage();
          return;
        }
      }
      else if(daysSelected[1] == 2){                                            //The buttons are hidden, If touch a button only select that one.
        if(awaitingButtonRelease(&dayBtns[i])){
          for(j = 0; j<numDayBtns; j++){                                        //all buttons off except...
            daysSelected[j] = 0;
          } 
          daysSelected[i] = 1;                                                  //button that was pressed.
          reDrawPage();
          return;
        } 
      }
    }
    return;
  }
  
  if(!updatePageOnly && !reDrawStuff){
    //Clear screen and set title
    tft.fillScreen(BLACK);
    tft.setTextSize(1);
    tft.setFont(&FreeSans9pt7b);
    centerTitle("Select days for schedule to apply", BLACK, WHITE);
    exitBtn.drawButton();
    tft.drawBitmap8(430, 2,timeline50x50, 50, 50, OPBOX_GREEN, 1);           //exit page btn
    reDrawStuff = true;
  } 
  
  if(reDrawStuff){
    tft.setFont(&FreeSans9pt7b);
    //tft.setFont(&FreeSmallFont);
    for(i = 0; i<numDayBtns; i++){
      dayBtns[i].drawButton();
    }
  }
}

void weeksSelectPage(){
  currentPage = 4;
  
  int i,j;
  
  for(i = 0; i<numWeekBtns; i++){                                              //Set button colours based on if ON or OFF or HIDDEN
    if((i == 0) && (weeksSelected[0] == 1)){             
      weekBtns[i].setFill(onFill);
      weekBtns[i].setOutline(onOutline);
      for(j = 1; j<numWeekBtns; j++){
        weeksSelected[j] = 2;                                                  //hidden
        weekBtns[j].setFill(offFill);
        weekBtns[j].setOutline(offOutline);
        weekBtns[j].setTextcolor(hiddenTextcolor);
      }
      break;
    }
    if(weeksSelected[i] != 1){
      weekBtns[i].setFill(offFill);
      weekBtns[i].setOutline(offOutline);
      weekBtns[i].setTextcolor(WHITE);
    }
    else if(weeksSelected[i] == 1){
      weekBtns[i].setFill(onFill);
      weekBtns[i].setOutline(onOutline);
      weekBtns[i].setTextcolor(WHITE);
    }
  }

  if(checkTPOnly){ 
    if(awaitingButtonRelease(&exitBtn)){
      disablePageLimits();                                                    //disable page limits that were used for this page but no longer valid for building next page from fresh
      schedulePage();                                                         //draw a fresh page
    }
    for(i = 0; i<numWeekBtns; i++){
      if((i == 0) && awaitingButtonRelease(&weekBtns[i])){                         //Look at the All button first
        if(weeksSelected[0] == 0){
          weeksSelected[0] = 1;                                                //All weeks are now selected
          for(j = 1; j<numWeekBtns; j++) weeksSelected[j] = 2;
        }
        else if(weeksSelected[0] == 1){
          weeksSelected[0] = 0;                                                ////All weeks are now de-selected
          for(j = 1; j<numWeekBtns; j++) weeksSelected[j] = 1;
        }
        reDrawPage();
        return;
      }
      else if(weeksSelected[1] != 2){                                            //Now lets look at the lower res buttons if they are not hidden
        int numWeeksSelected = 0;
        int lastWeek;
        for(j = 1; j<numWeekBtns; j++){
          if(weeksSelected[j] == 1){
            numWeeksSelected++;
            lastWeek = j;
          }
        }
        if(numWeeksSelected<1){                                                  //should never happen
          weeksSelected[0] = 1;
          reDrawPage();
          return;
        }
        if((numWeeksSelected == 1) && weekBtns[lastWeek].contains(tp.x,tp.y)){     //Dont disable the last remaining week button to be turned off
          return;
        }
        if(awaitingButtonRelease(&weekBtns[i])){
          weeksSelected[i] = !weeksSelected[i];
          reDrawPage();
          return;
        }
      }
      else if(weeksSelected[1] == 2){                                            //The buttons are hidden, If touch a button only select that one.
        if(awaitingButtonRelease(&weekBtns[i])){
          for(j = 0; j<numWeekBtns; j++){                                        //all buttons off except...
            weeksSelected[j] = 0;
          } 
          weeksSelected[i] = 1;                                                  //button that was pressed.
          reDrawPage();
          return;
        } 
      }
    }
    return;
  }
  
  if(!updatePageOnly && !reDrawStuff){
    //Clear screen and set title
    tft.fillScreen(BLACK);
    tft.setTextSize(1);
    tft.setFont(&FreeSans9pt7b);
    centerTitle("Select weeks for schedule to apply", BLACK, WHITE);
    exitBtn.drawButton();
    tft.drawBitmap8(430, 2,timeline50x50, 50, 50, OPBOX_GREEN, 1);           //exit page btn
    reDrawStuff = true;
  } 
  
  if(reDrawStuff){
    tft.setFont(&FreeSans9pt7b);
    //tft.setFont(&FreeSmallFont);
    for(i = 0; i<numWeekBtns; i++){
      weekBtns[i].drawButton();
    }
  }
}

void maxTimeSlotsPage(){
  currentPage = 6;
  
  Adafruit_GFX_Button okBtn;
  okBtn.initButtonUL(&tft, xCenter-30, yDisp-31, 60, 30, DARKGREY, BLACK, WHITE, "Okay", 2);
  if(checkTPOnly){                                                        //only checking for updating when a button is pressed on the page
    if(awaitingButtonRelease(&okBtn)){
      disablePageLimits();
      schedulePage();
    }
    return;                                                                 
  }

  if(!updatePageOnly && !reDrawStuff){                                    //only runs if we are building page from scratch, enters here if you call the page function directly without going through updatePage()
    tft.fillScreen(OFFBLACK); 
    tft.setFont(&FreeSans9pt7b);
    okBtn.drawButton();

    printTextBoxUL(20, 20, WHITE, BLACK, WHITE, "Oops, the max number of timeslots are currently being used. \n\n Please remove an existing one, to specify a new slot."); 
  }
}

void drawSelectedTime(){
    //tft.setFont(NULL);
    tft.setFont(&FreeSans24pt7b);                                                                        //The new timeslot time
    char buff[6];
    strcpy(buff, "");
    sprintf(buff, "%02d:%02d", newTSTime.hour, newTSTime.minute);
    centerPrintCustomFont(buff, yCenter, WHITE, BLACK, 2);
}

void newTimeslotPage(){
  currentPage = 5;

  if(numSlots == maxNumSlots){
    maxTimeSlotsPage();
    return;
  }

  Adafruit_GFX_Button downHourBtn, downMinuteBtn, upHourBtn, upMinuteBtn  ;                                               //up and down hour:minute buttons
  upHourBtn.initButtonUL(&tft,  120, 45, 110, 51, DARKGREY, BLACK, WHITE, "", 1, 1, upArrow100x50, 100, 50);
  upMinuteBtn.initButtonUL(&tft,  260, 45, 110, 51, DARKGREY, BLACK, WHITE, "", 1, 1, upArrow100x50, 100, 50);
  downHourBtn.initButtonUL(&tft,  120, 190, 110, 51, DARKGREY, BLACK, WHITE, "", 1, 1, downArrow100x50, 100, 50);
  downMinuteBtn.initButtonUL(&tft,  260, 190, 110, 51, DARKGREY, BLACK, WHITE, "", 1, 1, downArrow100x50, 100, 50);

  int timeSlotColor = CYAN;
  
  int xMinTL = 10;                                                      //foundational margin dimensions for timeline bar, and timeslot
  int xMaxTL = xDisp-10;
  int yMinTL = yDisp-20;
  int yMaxTL = yDisp-40;
  
  if(checkTPOnly){                                                      //only checking for updating when a button is pressed on the page
    if(awaitingButtonRelease(&exitBtn)){
      if(!timeSlotExists(newTSTime)){
        insertTimeSlot(newTSTime);
      }
      disablePageLimits();                                              //disable page limits that were used for this page but no longer valid for building next page from fresh
      schedulePage();                                                   //draw a fresh homepage
      return;
    }
    //Below is only for adjusting time
    tft.setFont(&FreeSans9pt7b);
    tft.setTextSize(4);
    if(buttonIsPressed(&upHourBtn)){
      newTSTime.hour += 1;
      drawSelectedTime();
      if(newTSTime.hour >= 24) newTSTime.hour = 0;
      if(buttonIsHeld(&upHourBtn, 700)){
        while(buttonIsHeld(&upHourBtn, 75)){
          newTSTime.hour += 1;
          if(newTSTime.hour >= 24) newTSTime.hour = 0;
          drawSelectedTime();
        }
      }
      reDrawPage();
      upHourBtn.drawButton();
      return;
    }
    
    if(buttonIsPressed(&upMinuteBtn)){
      newTSTime.minute += 1;
      drawSelectedTime();
      if(newTSTime.minute >= 60) newTSTime.minute = 0;
      if(buttonIsHeld(&upMinuteBtn, 700)){
        while(buttonIsHeld(&upMinuteBtn, 75)){
          newTSTime.minute += 1;
          if(newTSTime.minute >= 60) newTSTime.minute = 0;
          drawSelectedTime();
        }
      }
      reDrawPage();
      upMinuteBtn.drawButton();
      return;
    }
    
    if(buttonIsPressed(&downHourBtn)){
      newTSTime.hour -= 1;
      drawSelectedTime();
      if(newTSTime.hour < 0) newTSTime.hour = 23;
      if(buttonIsHeld(&downHourBtn, 700)){
        while(buttonIsHeld(&downHourBtn, 75)){
          newTSTime.hour -= 1;
          if(newTSTime.hour < 0) newTSTime.hour = 23;
          drawSelectedTime();
        }
      }
      reDrawPage();
      downHourBtn.drawButton();
      return;
    }
    
    if(buttonIsPressed(&downMinuteBtn)){
      newTSTime.minute -= 1;
      drawSelectedTime();
      if(newTSTime.minute < 0) newTSTime.minute = 59;
      if(buttonIsHeld(&downMinuteBtn, 700)){
        while(buttonIsHeld(&downMinuteBtn, 75)){
          newTSTime.minute -= 1;
          if(newTSTime.minute < 0) newTSTime.minute = 59;
          drawSelectedTime();
        }
      }
      reDrawPage();
      downMinuteBtn.drawButton();
      return;
    }
    return;                                                                 
  }

  if(!updatePageOnly && !reDrawStuff){                                                    //only runs if we are building page from scratch, enters here if you call the page function directly without going through updatePage()
    tft.fillScreen(BLACK);                                                                //Clear screen to remove previous artifacts
    tft.setTextSize(1);
    centerTitle("New Timeslot", BLACK, BLACK);
    exitBtn.drawButton();
    tft.drawBitmap8(430, 2, check50x50, 50, 50, GREY, 1); 

    upHourBtn.drawButton();
    upMinuteBtn.drawButton();
    downHourBtn.drawButton();
    downMinuteBtn.drawButton();

    reDrawStuff = true;
  }

  if(reDrawStuff){
    tft.fillRect(0, yMaxTL-10, xDisp, yDisp, BLACK);                                                  //Clear All below Exit button
    
    drawSelectedTime();                                                                               //new timeslot time
    
    tft.drawLine(xMinTL, yMaxTL, xMaxTL, yMaxTL, GREY);
    tft.drawLine(xMinTL, yMinTL, xMaxTL, yMinTL, GREY);

    int color = 255;
    int inSet = -30;
    for(int a = inSet ;a <= xMinTL; a++){
      int grad = map(a,inSet,xMinTL,128,255);                                                                 //colour map - maps value (currentData, fromLow, fromHigh, toLow, toHigh)t
      tft.drawLine(xMinTL+(a), yMinTL-1, xMinTL+(a), yMaxTL+1, tft.color565(255-grad, 255-grad, 255-grad));   //Left most gradient
      tft.drawLine(xMaxTL-(a), yMinTL-1, xMaxTL-(a), yMaxTL+1, tft.color565(255-grad, 255-grad, 255-grad));   //Right most gradient
    }
    
    for(int i = 0; i<numSlots; i++){                                                                          //The timeSlot bars vertical line and time stamps
      int seconds = secondsSinceNewDay(startTimeSlots[i]);
      int x = map(seconds,0,secondsInDay,xMinTL,xMaxTL);                                                      //x slot map - maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
      int rectLeft = x -19;
      sprintf(strTemp, "%02d:%02d", startTimeSlots[i].hour, startTimeSlots[i].minute);
      tft.drawLine(x, yMinTL+5, x, yMaxTL-5, timeSlotColor);
      tft.fillRectUL(rectLeft, yMinTL+5, 38, 12, BLACK);
      tft.drawRectUL(rectLeft, yMinTL+5, 38, 12, timeSlotColor);
      tft.setFont(NULL);
      tft.setTextSize(0.5);
      tft.print(strTemp, rectLeft+5, yMinTL+7 );
    }
    return;
  }
  int seconds = secondsSinceNewDay(newTSTime);                                  //new timeslot time visualization
  int xCand = map(seconds,0,secondsInDay,xMinTL,xMaxTL);
  tft.setColor(WHITE);
  tft.drawLine(xCand, yMinTL+5, xCand, yMaxTL-5);
  tft.drawLine(xCand, yMaxTL-5, xCand-4,yMaxTL-9);
  tft.drawLine(xCand, yMaxTL-5, xCand+4,yMaxTL-9);
  int msAnimation = 100;
  for(int i = 0 ; i<msAnimation; i++){                                          //Fade to black animation
    if(touchIsPressed()) break;                                                 //if there is a touch leave animation and handle it
    int col = map(i,0,msAnimation,0,255);                                       //maps value (currentTemp, fromLow, fromHigh, toLow, toHigh)
    tft.setColor(255-col, 255-col, 255-col);
    tft.drawLine(xCand, yMinTL+5, xCand, yMaxTL-5);
    tft.drawLine(xCand, yMaxTL-5, xCand-4,yMaxTL-9);
    tft.drawLine(xCand, yMaxTL-5, xCand+4,yMaxTL-9);
  }
  tft.setColor(WHITE,WHITE);
  tft.drawLine(xCand, yMinTL+5, xCand, yMaxTL-5);
  tft.drawLine(xCand, yMaxTL-5, xCand-4,yMaxTL-9);
  tft.drawLine(xCand, yMaxTL-5, xCand+4,yMaxTL-9);
                                                                                //below sets/updates rest of page every time the main loop repeats
}
