/*********************************************************************************************
   This is the I2C slave of "Brau Maschine Next Gen".
   It is controled by the corresponding master via I2C.

   The communication consists of two types of control commands
    - Status request
    - Set Commands

   A Set Command consist of one step
   - master sens the command string (i.e. switch relais) and slave answers with
     an error code byte.

   A Status requests consists of 2 steps:
    1. The master sends the control byte CMD_PREP_STATUS and the slave prepares the answer string.
    The slave response consists of one byte representing the length of the status string.

    2. The master sends the control byte CMD_SEND_STATUS and the slave responses
    with the status string prepared in step 1.

*/

#include <Wire.h>
#include "LCD.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <stdlib.h>

#define ONE_WIRE_BUS 4


#define DEBUG
#define MAX_BUF 100

#define REQ "REQ"

#define CMD_NONE 0
#define CMD_PREP_STATUS 1
#define CMD_SEND_STATUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


//receive complete string
volatile char buffReadString[MAX_BUF];

//receive single byte
volatile byte buffReadByte;

//when true: there was a command received not being handled yet
volatile bool bCmdReceived;

//this buffer is filled within loop and sent on master#s request
char buffSend[MAX_BUF];

String strStatus;

// on master#s request: send a byte or string?
bool bWriteByte;

byte errorCode = 0;


LCD lcd;




/**********************************************************
   Prepare the status string to be sent later
*/
int prepareStatusString() {
  static int i = 0;
  if (i == 0)
    strStatus = "0;R1=68;15";
  if (i == 1)
    strStatus = "1;R1=34;125.345";
  if (i == 2)
    strStatus = "2;R1=0;25.34896565";

  if (++i > 2)
    i = 0;

  return strStatus.length();
}




/**********************************************************
   Called whenever something was received on I2C.
   Interrupt handler!
*/
void receiveEvent(int howMany) {
  //Serial.print("howMany: '"); Serial.print(howMany);  Serial.println("'");
  int i = 0;

  //one command byte
  if (howMany == 1) {

    //TODO: Is it ok to ask just for one byte? Check it ...
    buffReadByte = Wire.read();
    while (0 < Wire.available()) {
      char c = Wire.read();
      Serial.println(F("ERROR - still somthing in buffer?!"));
    }
  }

  else { //handle the command string

    buffReadByte = CMD_NONE;   //a string command is answered by byte
    while (0 < Wire.available()) {
      char c = Wire.read();      /* receive byte as a character */

      //Serial.print(c);           /* print the character */
      if (i < MAX_BUF) {
        buffReadString[i] = c;
        i++;
      }
      else
        Serial.println(F("ERROR - buffer overload!"));
    }
    buffReadString[i] = 0;  //Finish string
  } // else { //handle the command string

  //remember in loop there is a command to be handled
  bCmdReceived = true;

  /*
    #ifdef DEBUG
    String s = buffRead;
    Serial.print("I2C received: '"); Serial.print(s);  Serial.println("'");
    #endif
  */
}



/**********************************************************
   Function called when I2C master requests data.
   String to be sent must be prepared before.
*/
void requestEvent() {

  //Wire.write("I2C response", 12);  /*send string on request */
  if (bWriteByte)
    Wire.write(buffSend[0]);
  else
    Wire.write(buffSend, strlen(buffSend));  /*send string on request */
  //Serial.println(buffSend);
}


/**********************************************************
   Setup.
*/
void setup() {

  lcd.show ("Initializing", "", "", "");

  Wire.begin(1);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  Serial.begin(115200);           /* start serial for debug */
  Serial.println(F("--- SETUP ---"));

  // bCmdInWork = false;
  bCmdReceived = false;
  strcpy(buffSend, "");
  buffReadByte = CMD_NONE;
  strcpy (buffReadString, "");

  sensors.begin();

  lcd.show ("Initializing", "Done", "", "");
  delay(500);
}


/**********************************************************
   Loop.
*/
void loop() {
  unsigned long t1 = millis();
  sensors.requestTemperatures(); // Send the command to get temperature readings
  String strTemp (sensors.getTempCByIndex(0));

#ifdef DEBUG
  Serial.print(F("Time "));
  Serial.print(millis() - t1);
  Serial.print(F("ms,  T="));
  Serial.print(strTemp.c_str());
  Serial.println("°C");
#endif

  static byte b = 170;

  strTemp = "Sud: " + strTemp + ((char)176) + 'C';
  
  String //s1 = String(++b) + ":" + ((char) b),
         s2 = String(++b) + ":" + ((char) b),
         s3 = String(++b) + ":" + ((char) b),
         s4 = String(++b) + ":" + ((char) b);
 delay(500);


  lcd.show (strTemp.c_str(), s2.c_str(), s3.c_str(), s4.c_str());

  //check if a command was received. If yes -> prepare the answer
  if (bCmdReceived) {
    bWriteByte = true;  //a byte is answered by one byte

    String strBuffer = String(buffReadByte);

    // === A 'Set Command' received ===

    if (buffReadByte == CMD_NONE)
    {
      strBuffer = buffReadString;
      //TODO: Handle string
    }

    // === A 'Status Request' command byte received ===

    //prepare a status string, prepare length for next request
    else if (buffReadByte == CMD_PREP_STATUS)
    {
      int len = prepareStatusString();
      if (len > 255) {
        Serial.print(F("String to be sent exceeds limit of 255!"));
        errorCode = 1;
        len = 255;
      }
      buffSend[0] = (byte) len;
    }

    //send the prepared status
    else if (buffReadByte == CMD_SEND_STATUS)
    {
      bWriteByte = false;  //a byte is answered by string
      strcpy (buffSend, strStatus.c_str());
    }

    else {
      Serial.print(F("Don't know how to handle command byte '"));
      Serial.print(buffReadByte);
      Serial.println(F("'"));
    }

#ifdef DEBUG
    Serial.println();
    Serial.print(F("Received: '")); Serial.print(strBuffer);
    if (bWriteByte) {
      Serial.print(F("', Sent byte '"));
      Serial.print ((int) buffSend[0]);

    }
    else {
      Serial.print(F("', Sent string '"));
      strBuffer = buffSend;
      Serial.print(strBuffer);
    }
    Serial.println(F("'"));
#endif

    bCmdReceived = false;
  }
  //else
  //  Serial.print(F("."));
}
