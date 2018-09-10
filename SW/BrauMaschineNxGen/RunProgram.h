#include "Arduino.h"
#include "DeviceState.h"
#include "ErrorHandler.h"
#include "HAL.h"
#include <TimeLib.h>

#ifndef run_prog_h
#define run_prog_h


// ------------- other ------------------
#define DEBUG_RAST
#define MAX_RASTEN  7    //max Number of Rasten



// ----------------- Program Parameter -----------------------

struct brewType {
  const char *name;       //Name prefix of HTML input elements
  unsigned char temp;     //Temperature limit
  unsigned char time;     //max time for "rast"
  unsigned char raise;    //raise of temperature increase
  bool wait4User;         //Shall it continue automatically or wait for user interaction
};


//extern byte rast_num_current;  //the current rast-number starting with 0. Is 255 if no rast is active
//extern unsigned long timer_rast_target;  //contains the calculated target time

// ----------------- Protocol -----------------------


struct protocolType {
  //Unix Time
  unsigned long timeStart;            // using millis()()
  unsigned long timeStop;           // using millis()

  unsigned long timeHeatingStart[MAX_RASTEN];  //When the rast step  was started by heating
  unsigned long timeRastStart[MAX_RASTEN];   //When the rast step reached the target temp and the rast pahse started (first time)
};



// ----------------- Protocol -----------------------

class RunProgram {

  private:

    const float hysterese_sud = 0.3;    //used for temp regulation
    unsigned long timer_rast_target;

    bool wasRunning;
    bool isRunProceedRequested;

    byte rast_num_current;  //0: no running, 1..7


    protocolType protocol;
    //bool rast_halt_executed;

    DeviceState* pDeviceState;
    ErrorHandler* pErrorHandler;
    HAL* pHal;

    void doHeating();
    void doRasting();
    void doWaiting();

#ifdef DEBUG_RAST
    void debugRast(String s);
 #endif

    public:
      brewType rasten[MAX_RASTEN];

      RunProgram(DeviceState * d, ErrorHandler * h, HAL * hal);
      byte getCurrentRastNum();
      brewType getRast(byte num);
      protocolType getProtocol();

      void proceedRunRequested();

      void loop();

  };
#endif
