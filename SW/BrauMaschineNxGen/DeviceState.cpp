
/**
   Device State
*/
#include "Arduino.h"
#include "DeviceState.h"


#define STR_CHANGE_REQUESTED "state change requested ..."


DeviceState::DeviceState(ErrorHandler h)
{
  errorHandler = h;
  deviceState = STATE_INIT;
  simulation = true;
}

// --- Current State ----
bool DeviceState::isInit() {
  return deviceState == STATE_INIT;
}

bool DeviceState::isSetup() {
  return deviceState == STATE_SETUP;
}

bool DeviceState::isRunning() {
  return deviceState == STATE_RUNNING;
}

bool DeviceState::isRunning_Heating() {
  return isRunning() && deviceSubState == SUBSTATE_RUNNING_HEATING;
}

bool DeviceState::isRunning_Rasting() {
  return isRunning() && deviceSubState == SUBSTATE_RUNNING_RASTING;
}

bool DeviceState::isRunning_Waiting() {
  return isRunning() && deviceSubState == SUBSTATE_RUNNING_WAITING;
}



bool DeviceState::isFinished() {
  return deviceState == STATE_FINISHED;
}

bool DeviceState::isPaused() {
  return deviceState == STATE_PAUSED;
}

bool DeviceState::isSimulating() {
  return simulation;
}
void DeviceState::setSimulation(bool b) {
  simulation = b;
}

// --- Switch State ---

// Request state change. The state change will be performed within loop().

void DeviceState::requestToSetup() {
  requestedState = STATE_SETUP;
}

void DeviceState::requestToRun() {
  Serial.println ("=== DeviceState::requestToRun() ===");  
  requestedState = STATE_RUNNING;
  requestedSubState = SUBSTATE_RUNNING_HEATING;
}

void DeviceState::requestToRun_Rasting() {

Serial.println ("=== DeviceState::requestToRun_Rasting() ===");  
  requestedSubState = SUBSTATE_RUNNING_RASTING;
}


void DeviceState::requestToRun_Waiting() {
Serial.println ("=== DeviceState::requestToRun_Waiting() ===");   
  requestedSubState = SUBSTATE_RUNNING_WAITING;
}

void DeviceState::requestToFinish() {
  requestedState = STATE_FINISHED;
}

void DeviceState::requestToPause() {
  requestedState = STATE_PAUSED;
}




// ---
/************************************************
   Check if a triggered state change is allowed
*/
void DeviceState::loop() {

  if ((requestedState == deviceState)  && (requestedSubState == deviceSubState))
    return; //ignore

  bool doit = false;

  // --- Switch Main State ---
  if (requestedState != deviceState)    //is switching of main state allowed?
  {
    if ( (requestedState == STATE_SETUP)
         && (isInit() || isFinished() || isSetup())) {
      doit = true;
    }
    else if ((requestedState == STATE_RUNNING)
             && (isSetup() || isPaused())) {
      requestedSubState = SUBSTATE_RUNNING_HEATING;   //force substate triggering
      deviceSubState = SUBSTATE_RUNNING_HEATING;
      doit = true;
    }
    else if ((requestedState == STATE_FINISHED)
             && isRunning()) {
      doit = true;
    }
    else if ((requestedState == STATE_PAUSED)
             && isRunning()) {
      doit = true;
    }
  } //if (requestedState != deviceState)


  // --- Switch Sub State only , no MainState change---
  if (requestedSubState != deviceSubState) {
    doit = true;
  }

  if (doit) {
#ifdef DEBUG_DEV_SSTATE
    Serial.print(F("DeviceState::loop((): Switching State FROM ")); Serial.print(getStateAsString());
#endif

    deviceState = requestedState;
    deviceSubState = requestedSubState;

#ifdef DEBUG_DEV_SSTATE
    Serial.print(F(" TO ")); Serial.println(getStateAsString());
#endif
  }

  else {
    String s = F("Cannot switch device state from ");
    s += getStateAsString();
    s += F(" to [main state='");
    s += String(requestedState);
    s += F("', substate='");
    s += String(requestedSubState);
    s += F("'].");
    errorHandler.handleError(s);
    requestedState = deviceState;
  }
}


String DeviceState::getStateAsString() {
  String s = F("[main state='");
  s += String(deviceState);
  s += F("', substate='");
  s += String(deviceSubState);
  s += F("']");
  return s;
}




