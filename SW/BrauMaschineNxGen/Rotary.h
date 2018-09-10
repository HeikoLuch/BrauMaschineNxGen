#include "Arduino.h"

#ifndef rotary_h
#define rotary_h

#define DEBUG_ROTARY
/********************************************************
 * Class providing infos for rotary encoder.
 * - keeps info if button was pressed OR rotated
 * - Start a timer when button was pressed; After certain time, the buffer is cleared.
 */

 #define BUTTON_TIMER 500
 
class Rotary {
	
private:
 	byte pin_clk;
 	byte pin_dt;
 	byte pin_sw;
 
	unsigned long btnTime;  //0 if button wasn't pressed
	
	int counter; 
 	int aState;
 	int aLastState; 
	
public:
	
	Rotary(byte, byte, byte);
	bool wasSwitched();
	//bool wasRotated();
	int getCounter();
	
	bool loop();
};



#endif
