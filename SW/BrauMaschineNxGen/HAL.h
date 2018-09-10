#include "Arduino.h"
#include "DeviceState.h"
#include "ErrorHandler.h"
#include "Wire.h"

#ifndef hal_h
#define hal_h

#define DEBUG_HAL

#define LOOP_INTVL 5000
#define ADDR_NANO 1
#define RCV_BUF_LEN 100
#define DELIMITER ';'

/****************************************************
   Hardware abstraction layer

   This class is responsible for communication with Arduino Nano
   to control the hardware being connected to the Nano.

   It knows two kind of commands: SET and RECEIVE.

   SET is sending a command to the Nano being executed (i.e. switch a relais).
   RECEIVE is requesting data from Nano (i.e. temperature value or a complete status dump).

   On communication error, the Error handler ins informed. The slave (Nano) must ensure to
   switch off all safety relevant components.

   From the HAL perspective the loop() method s called to communicate with the slave permanently
   fetching all necessary data.
   When asking for data keep i mind thatthese data are the last read data.
*/

#define CMD_NONE 0
#define CMD_PREP_STATUS 1
#define CMD_SEND_STATUS 2

class HAL {

  private:
    DeviceState* pDeviceState;
    ErrorHandler* pErrorHandler;

    int code_error;
    int code_relais_sud;



    //just for simulation
    float simulationTemp;

    // --- I2C communication ---
    //const char* SET = "SET";
    //const char* REQ = "REQ";

    unsigned long nextLoop = 0;       //timer within loop
    char bufReceived[RCV_BUF_LEN];    //contains last read string

    bool i2cReceiveString(int);  //false -> error
    byte i2cReceiveByte();  
    void i2cSend(const char*);
    void i2cSend(byte);
    void check4Error(byte);

    float tempSudValue;
    float tempInnerValue;
    float tempLautValue;

  public:

    HAL(DeviceState *d, ErrorHandler* h);
    void loop();

    // --- values read from slave ---
    bool relais_SUD;
    float relais_LAUT;
    float relais_PUMP;

    // --- Relais ---
    void switchOff_SUD();
    void switchOff_LAUT();
    void switchOff_PUMP();

    void switchOn_SUD();
    void switchOn_LAUT();
    void switchOn_PUMP();

    bool getRelaisStatus_SUD();
    bool getRelaisStatus_LAUT();
    bool getRelaisStatus_PUMP();

    // --- Temperature ---
    float getTempSudValue();
    float getTempInnerValue();
    float getTempLautValue();


    // --- Buzzer ---
    void buzzer_rast_start();
    void buzzer_rast_finished();

};

#endif
