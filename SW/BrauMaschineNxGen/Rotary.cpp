#include "Rotary.h"

Rotary::Rotary(byte _pin_clk, byte _pin_dt, byte _pin_sw)
{
  pin_clk = _pin_clk;
  pin_dt = _pin_dt;
  pin_sw = _pin_sw;

  counter = 0;
  btnTime = 0;

  pinMode (pin_clk, INPUT);
  pinMode (pin_dt, INPUT);
  pinMode (pin_sw, INPUT);

  // Reads the initial state of the outputA
  aLastState = digitalRead(pin_clk);
}


/***************************************************** 
 *  
 *  Returns true if switched or rotated.
 *  
 */
bool Rotary::loop() {
  boolean bRet = false;
  aState = digitalRead(pin_clk); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState) {
    bRet = true;
    // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead(pin_dt) != aState) {
      counter ++;
    } else {
      counter --;
    }
#ifdef DEBUG_ROTARY
    Serial.print("Position: ");
    Serial.println(counter);
#endif
  }
  aLastState = aState; // Updates the previous state of the outputA with the current state

  if (LOW == digitalRead(pin_sw)) {
   //Serial.println("LOW");
#ifdef DEBUG_ROTARY
    if (btnTime == 0)
      Serial.println("Rotary SW was pressed.");
#endif
    btnTime = millis() + BUTTON_TIMER;
    bRet = true;
  }

  
  if (millis() > btnTime){   //clear button buffer
    //Serial.println("HIGH");
    btnTime = 0;
  }
  return bRet;
}

/************************************************
 * 
 *
bool Rotary::wasRotated() {

}*/

/************************************************
 * True if the Button was pressed.
 * Clears the buffer.
 */
bool Rotary::wasSwitched() {
  if (btnTime > 0) {
    btnTime =0;
    return true;
  }
  return false;
}


int Rotary::getCounter() {
  return counter;
}

