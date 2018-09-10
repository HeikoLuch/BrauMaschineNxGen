#include "Arduino.h"
#include "ErrorHandler.h"

#ifndef dev_state_h
#define dev_state_h

#define DEBUG_DEV_SSTATE

// ======================== Main States =======================
//on setup()
#define STATE_INIT 1

//input data for brewing
#define STATE_SETUP 2

//run the current brew process
#define STATE_RUNNING 3

//brew run successfully finished
#define STATE_FINISHED 4

//pause the current brew process
#define STATE_PAUSED 5

// ======================== Running SubStates =======================

//subs state for all states without assigned substate
#define SUBSTATE_DEFAULT1 1

#define SUBSTATE_RUNNING_HEATING 1
#define SUBSTATE_RUNNING_RASTING 2
#define SUBSTATE_RUNNING_WAITING 3

class DeviceState {
	
private:
  
  byte requestedState;        //the state to be switched to
  byte requestedSubState;     //the substate to be switched to
  bool simulation;
  
  ErrorHandler errorHandler;
	
public:
	byte deviceState;           //current state
  byte deviceSubState;        //current state
  
 	DeviceState(ErrorHandler h);

  // --- Ask for current state ---

  bool isInit();
  bool isSetup();
  
  bool isRunning();
  bool isRunning_Heating();
  bool isRunning_Waiting();
  bool isRunning_Rasting();
  
  bool isFinished();
  bool isPaused();

  // --- Request for  state change ---

  void toSetup();
  
  void requestToRun();  //Starts heating too
  void requestToRun_Rasting();
  void requestToRun_Waiting();
  
  void requestToFinish();
  void requestToPause();
  void requestToSetup();

  // ---
  bool isSimulating();
  void setSimulation(bool b);
  void loop();

  // Just for error handling
  String getStateAsString();
/*
  //after init or when programm is done -> Input State
  void startInput();

  //start a selected program
 	void startRun();

  //cancel current program and return to Input State
  void cancelRun();

  //pause current program 
  void pauseRun();

  //proceed after program pause
  void proceedRun();

  //when running -> rast achieved. Wait for user interaction
  void startRast();
  //after user interaction, the programm is proceeded
  void cancelRast();
  */
};



#endif
