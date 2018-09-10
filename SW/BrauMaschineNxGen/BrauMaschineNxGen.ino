//#include "NTP-Clock.h"
#include "WebInterface.h"
#include "DeviceState.h"
#include "ConnectWifi.h"
#include "Rotary.h"


//Time Offset for time zone
#define BLN_TIME 2
#define PIN_LED 2

 #define PIN_CLK 13
 #define PIN_DT 12
 #define PIN_SW 14
 
NTP ntp (BLN_TIME);
ErrorHandler errorHandler;
DeviceState deviceState (errorHandler);    //device state manager of BM
HAL hal(&deviceState, &errorHandler);
RunProgram runProgram (&deviceState, &errorHandler, &hal);
Memory memory;
ConnectWifi connWifi;

Rotary rotary (PIN_CLK, PIN_DT, PIN_SW);


// ------------- Rotary ---------------
// D7..13, D6 .. 12, D5 .. 14



void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  Serial.begin(115200);
  Serial.println("=== Braumaschine NextGen ===");
  connWifi.checkConnection();
  //ntp_init();
  deviceState.requestToSetup();
  wsInit(&deviceState, &memory, connWifi.ipString, &ntp);
  digitalWrite(PIN_LED, HIGH);
 // r_setup();
}





void loop() {

  unsigned long zeitLoop = millis();

  connWifi.checkConnection(); //takes 2 ms when status == WL_CONNECTED
  //Serial.print ("Loop check WIFI time: "); Serial.print(millis() - zeitLoop); Serial.println( "ms");

  if ( rotary.loop()) {
  }
  
  ntp.loop(); //Ask for time

  hal.loop(); //request and receive data from hal
  deviceState.loop(); //handle state changes

  runProgram.loop();  //handle the program run - only if run state is active

  wsLoop(&runProgram); //show the current info on web page
  //delay(1); //auch: yield()

  /*
    if (deviceState.isConfiguring()) {
    }
    else if (deviceState.isRunning()) {
    }*/

  //Serial.print ("Loop summary time: "); Serial.print(millis() - zeitLoop); Serial.println( "ms");
}
