#include "timerFanControl.h"

void pwmHS_AUX()
{
  // setPin(ACT_HS_AUX, 0);
  HS_AUX_STATE = 0;
  //  Timer0.stop();
}
void toggleHS_AUX()
{
  if (HS_AUX_STATE)
  {
    // setPin(ACT_HS_AUX, 0);
    HS_AUX_STATE = 0;
  }
  else
  {
    // setPin(ACT_HS_AUX, 1);
    HS_AUX_STATE = 1;
    //  Timer0.attachInterrupt(pwmHS_AUX).start(1000000 * 1); //turn off after a sec
  }
}
void setFanPWM(int percent)
{
}
