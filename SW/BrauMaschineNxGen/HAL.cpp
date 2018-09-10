#include "HAL.h"

/*********************************************************
   Constructor
*/

HAL::HAL(DeviceState *d, ErrorHandler* h) {
  pDeviceState = d;
  pErrorHandler = h;
  simulationTemp = 10.0;
  Wire.begin();
  nextLoop = 0;
}

/*******************************************************
   Call this within main loop

*/
void HAL::loop() {

  int delayTime = 10;
  if ((nextLoop - millis()) > LOOP_INTVL) {
    Serial.println ("------------------------------------------------------");
    nextLoop = LOOP_INTVL + millis();
    //Serial.print("nextLoop="); Serial.println(nextLoop);

    // Prepare state
    Serial.print("Prepare Status received: '");
    i2cSend (CMD_PREP_STATUS);
    delay (delayTime); //Give some time to prepare the answer
    byte len = i2cReceiveByte();  
    Serial.print ("Answer length is '"); Serial.print (len); Serial.println ("'");
 delay (delayTime);

    //receive state
    i2cSend (CMD_SEND_STATUS);
     delay (delayTime);
    if (!i2cReceiveString(len))   //store in bufReceived
    {
      pErrorHandler->handleError("Received invalid string");
    }
  }
}

// --- I2C communication with Slave ---



/*******************************************************
   Receive one byte and return it.
*/
byte HAL::i2cReceiveByte() {
  byte b = 255;
  Wire.requestFrom(ADDR_NANO, 1); /* request & read data of size 13 from slave */
 
  if (Wire.available()) {
    b = Wire.read();
  }
  if (Wire.available())
    pErrorHandler->handleError ("Received length isn't consistent.");
  return b;
}

/*******************************************************
   Receive. Set all measurements to its value.
   Limit the length of the answer (maxLen > 0) OR use the default length.
*/
bool HAL::i2cReceiveString(int maxLen) {

  //store if something was received; if not -> error
  boolean bReceived = false;

  if (maxLen <= 0)
    maxLen = RCV_BUF_LEN;

  Wire.requestFrom(ADDR_NANO, maxLen); /* request & read data of size 13 from slave */
  
  byte i = 0;
  while (Wire.available()) {
    bufReceived[i] = Wire.read();
    i++;
    bReceived = true;
  }
  if (bReceived) {
    bufReceived[i] = 0; //Finish string

#ifdef DEBUG_HAL
    String s = bufReceived;
    Serial.print(F("I2C received: '")); Serial.print(s); Serial.println("'");
    //Serial.print(F("  error code: '")); Serial.print(code_error); Serial.println("'");
    //Serial.print(F("  relais sud: '")); Serial.print(code_relais_sud); Serial.println("'");
    //Serial.print(F("  tempSudValue: '")); Serial.print(tempSudValue); Serial.println("'");
#endif
    return true;
  }
  else {
    pErrorHandler->handleError(F("Nothing received via I2C"));
    return false;
  }
}




void HAL::i2cSend(const char* s) {
  bufReceived[0] = 0; //Make the receive empty

  //Serial.print("i2cSend ("); Serial.print(s); Serial.println(")");

  Wire.beginTransmission(ADDR_NANO);
  Wire.write(s);
  check4Error( Wire.endTransmission());
}



void HAL::i2cSend(byte b) {
  bufReceived[0] = 0; //Make the receive empty

  //Serial.print("i2cSend ("); Serial.print(s); Serial.println(")");

  Wire.beginTransmission(ADDR_NANO);
  Wire.write(b);
  check4Error( Wire.endTransmission());
}




void HAL::check4Error(byte error) {
  if (error == 1)
  {
    pErrorHandler->handleError(F("ERR 1 (I2C): data too long to fit in transmit buffer."));
  }
  else if (error == 2)
  {
    pErrorHandler->handleError(F("ERR 2 (I2C): received NACK on transmit of address."));
  }
  else if (error == 3)
  {
    pErrorHandler->handleError(F("ERR 3 (I2C): received NACK on transmit of data."));
  }
  else if (error == 4)
  {
    pErrorHandler->handleError(F("ERR 4 (I2C): other error."));
  }
#ifdef DEBUG_HAL
  else if (error == 0) {
    Serial.print("I2C OK.");
  }
#endif
  else
    pErrorHandler->handleError("ERR unknown (I2C). Failed to ransmit I2C.");
}



// --- Relais ---

void HAL::switchOff_SUD() {
  //i2cSend ("SET R1=0");
}

void HAL::switchOff_LAUT() {
  //i2cSend ("SET R2=0");
}


void HAL::switchOff_PUMP() {
  //i2cSend ("SET R3=0");
}



void HAL::switchOn_SUD() {
  i2cSend ("SET R1=1");
}

void HAL::switchOn_LAUT() {
  i2cSend ("SET R2=1");
}


void HAL::switchOn_PUMP() {
  i2cSend ("SET R3=1");
}


// --- Temperatur ---

float HAL::getTempSudValue() {
  return tempSudValue;

  /*
    if (pDeviceState->isSimulating()) {
    simulationTemp = simulationTemp + 0.01;
    return simulationTemp;
    }
    else
    return 21.0;
  */
}
float HAL::getTempInnerValue() {
  return tempInnerValue;

}
float HAL::getTempLautValue() {
  return tempLautValue;
}


// --- Buzzer ---


void HAL::buzzer_rast_start() {
}

void HAL:: buzzer_rast_finished() {}
