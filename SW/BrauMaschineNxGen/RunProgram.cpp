#include "RunProgram.h"

/****************************************
   Process Manager

   Controls the brewing process.
*/

RunProgram::RunProgram(DeviceState *d, ErrorHandler* h, HAL *hal)
{
  //rast_halt_executed = false;
  pDeviceState = d;
  pErrorHandler = h;
  pHal = hal;

  wasRunning = false;
  isRunProceedRequested = false;
}

/***********************************************************
   Wait for user interaction.
   I user interaction has happened OR no WAIT is configured -> increase rast num and trigger heating.
   If the last rast has reached -> trigger FINISHED

*/
void RunProgram::doWaiting() {

#ifdef DEBUG_RAST
  debugRast(F("--- RunProgram::doWaiting() ---"));
#endif

  bool finishWait = false;

  if (rasten[rast_num_current].wait4User) {
    if (isRunProceedRequested)
      finishWait = true;
  }
  else {
    finishWait = true;
  }

  if (finishWait) {
    if (++rast_num_current < MAX_RASTEN) {
#ifdef DEBUG_RAST
      String s = String(F("Starting next rast number: "));
      s += String(rast_num_current);
      debugRast(s);
#endif

      protocol.timeHeatingStart[rast_num_current] = millis();
      timer_rast_target = 0;  //as long as heating is not finished
      pDeviceState->requestToRun();  //Starts heating too
    }
    else {
      pDeviceState->requestToFinish();
#ifdef DEBUG_RAST
      debugRast(F("RunProgram::doWaiting(): Requesting to finish."));
#endif
    }
  }
}



/***********************************************************
   Wait for Rast time.
   If timer is elapsed -> trigger next substate (WAITING)
*/

void RunProgram::doRasting() {

#ifdef DEBUG_RAST
  debugRast(F("--- RunProgram::doRasting() ---!"));
#endif


  if (millis() > timer_rast_target) {
#ifdef DEBUG_RAST
    debugRast(F("RunProgram::doRasting(): Timer elapsed!"));
#endif
    //protocol.timeRastStart[rast_num_current] = millis();
    isRunProceedRequested = false;
    pDeviceState->requestToRun_Waiting();//substate_main_requested = SUBSTATE_RUNNING_HALT;
  }
  else {
#ifdef DEBUG_RAST
    debugRast(F("RunProgram::doRasting(): Waiting for timer."));
#endif
  }
}
/*****************************************************************
   Handle the substate HEATING when running.

   Heat if temperature is not reached.
   If temperature is reached prepare and trigger rast.
*/

void RunProgram::doHeating() {

#ifdef DEBUG_RAST
  debugRast(F("--- RunProgram::doHeating() ---!"));
#endif

  const __FlashStringHelper* strLoc = F("RunProgram::heating()");

  float temp = pHal->getTempSudValue();
  if (  temp < ( rasten[rast_num_current].temp + hysterese_sud)) {

#ifdef DEBUG_RAST
    String s = String(F("RunProgram::doHeating(): Temperature is "));
    s += String(temp);
    debugRast(s);
#endif

    timer_rast_target = 0;
    pHal->switchOn_SUD();
  }

  else {
#ifdef DEBUG_RAST
    debugRast(F("RunProgram::doHeating(): Heating finished, starting rast"));
#endif
    pHal->switchOff_SUD();
    pDeviceState->requestToRun_Rasting();

    //calculate rast timeer
    protocol.timeRastStart[rast_num_current] = millis();
    if (pDeviceState->isSimulating())
      timer_rast_target = ((unsigned long) rasten[rast_num_current].time * 500) + millis();  //time == 0,5 seconds!
    else
      timer_rast_target = ((unsigned long) rasten[rast_num_current].time * 1000 * 60) + millis();
  }
}

/***************************************************************
   Call this within loop().
*/

void RunProgram::loop() {
  bool err = false;
  if (pDeviceState->isRunning()) {

    //the state has changed-> initialize the project
    if (!wasRunning) {
      rast_num_current = 0;
      wasRunning = true;
    }

    //heating
    if ( pDeviceState->isRunning_Heating()) {
      doHeating();
    }

    //rasting
    else if ( pDeviceState->isRunning_Rasting()) {
      doRasting();
    }

    //waiting
    else if ( pDeviceState->isRunning_Waiting()) {
      doWaiting();
    }

    else err = true;
  }
  else {
    //not Running -> switch everything off
    pHal->switchOff_SUD();
    pHal->switchOff_LAUT();
    pHal->switchOff_PUMP();

    wasRunning = false;
    if (pDeviceState->isSetup() || pDeviceState->isFinished()) {
      //nothing
    }
    else if (pDeviceState->isPaused()) {

    }
    else err = true;
  }


  if (err) {
    String s = F("Unknown state to handle: ");
    s += pDeviceState->getStateAsString();
    s += F(".");
    pErrorHandler->handleError(s);

    pHal->switchOff_SUD();
    pHal->switchOff_LAUT();
    pHal->switchOff_PUMP();
  }

}



// Infos regarding

byte RunProgram::getCurrentRastNum() {
  return rast_num_current;
}


brewType RunProgram::getRast(byte num) {
  return rasten[num];
}

protocolType RunProgram::getProtocol() {
  return protocol;
}


void RunProgram::proceedRunRequested() {
  isRunProceedRequested = true;
}


#ifdef DEBUG_RAST
void RunProgram::debugRast(String s) {

  Serial.print ("------ Rast #"); Serial.print(rast_num_current); Serial.println(" ------");
  Serial.print ("Event: "); Serial.println(s);
  Serial.print ("Millis(): "); Serial.println( millis());
  Serial.print ("Timer Target: "); Serial.println( timer_rast_target);
  Serial.print ("Protocol Heating Time: "); Serial.println( protocol.timeHeatingStart[rast_num_current]);
  Serial.print ("Protocol Rast Time: "); Serial.println( protocol.timeHeatingStart[rast_num_current]);

}
#endif



