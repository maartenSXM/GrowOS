#include "backupMode.h"
#include <stdio.h>

void setupBUMODE()
{
  gotPWROFF_INT = false;
  /*
  SUPC->SUPC_MR |= 0x00004000u;             //VDDIO is present set VDDIORDY bit on wakeup
  //Reports the cause of the last processor reset. Reading this RSTC_SR does not reset this field.
  // 0 = fresh 1 = backup mode reset 2 = watchdog , 3 = software, 4 = User/NSRT
  uint32_t resetStatus = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos;
#ifdef DEBUG
  printf("\nRSTTYP = 0x%02X\n", resetStatus); // Should be 0x01 see page 232
#endif
  //pinMode(A6, INPUT);                       //WKUP1
  SUPC->SUPC_SR;                              //to clear a WAKE UP Status?
  SUPC->SUPC_WUIR |= (0x1u << 15);            //WKUPEN1 enable the wakeup via WKUP15
  SUPC->SUPC_WUIR |= (0x1u << 31);            //Set transition of WKUP1 to be LOW_to_high
  SUPC->SUPC_WUIR |= (0x1u << 1);             //WKUPEN1 enable the wakeup via WKUP1
  SUPC->SUPC_WUIR |= (0x1u << 17);            //Set transition of WKUP1 to be LOW_to_high
  SUPC->SUPC_WUMR &= 0xFFFF8FFFu;             //set debounce to 0Seconds
  SUPC->SUPC_WUMR |= 0x1u;                    //enable FWUPEN 0Seconds
  SUPC->SUPC_WUMR |= (0x5u << 12);            //0x5u = 1second set debounce to 4096 (about 128ms) clock cycles before starting wakeup
  //SUPC->SUPC_SR //to clear a WAKE UP Status?.      WKUPIS1 = the status of it being either 0 (disabled) , or 1 (enabled) at the time the trigged wake up event????
  //isWKUP1 = (SUPC->SUPC_SR & SUPC_SR_WKUPIS1) >> 17 Can use WKUPIS_ to find out which input caused the wakeup. WKUPS - 0(NO) = no wake up due to the assertion of the WKUP pins has occurred since the last read of SUPC_SR.      1 (PRESENT) = at least one wake up due to the assertion of the WKUP pins has occurred since the last read of SUPC_SR.

  //12v power off interrupt
  attachInterrupt(INT_PWROFF, state_change, RISING);
  */
}

void state_change()
{
  if (!gotPWROFF_INT)
  {
    gotPWROFF_INT = true; // to avoid bouncing signal
    // SUPC->SUPC_MR &= 0xFFFFBFFFu;                 //clear VDDIORDY bit in SUPC_MR atleast 2 slow clock period before VDDIO voltage removed
    // detachInterrupt(digitalPinToInterrupt(46));   //disable power off interrupt so not to get multiple
    // printf("Entering backup mode : Do NOT stop the software now  |:\n");
    // state = (state + 1);
    delayMicroseconds(500);
    EnterBackupMode();
  }
}

void RTT_Handler()
{
  // RTT->RTT_SR;                                  //Reading the RTT_SR status register resets the RTTINC and ALMS fields.
}

void EnterBackupMode(void)
{
  // GPBR->SYS_GPBR[0] = GPBR->SYS_GPBR[0] + 1;

  // or alternate way to enter backup mode
  // SUPC->SUPC_CR = SUPC_CR_VROFF_STOP_VREG | SUPC_CR_KEY(SUPC_KEY);
  // http://ww1.microchip.com/downloads/en/AppNotes/Atmel-42160-Low-Power-Modes-in-SAM3-Family_Application-Note_AT03498.pdf
  // SCB->SCR |= (0x1u << 2);     //set a 1 to the sleepdepp bit in system control register
  //__WFE();                     //wait for interrupt or event
}
