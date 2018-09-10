#include "ErrorHandler.h"

ErrorHandler::ErrorHandler(){}
void ErrorHandler::handleError(String s){
  Serial.print ("EVENT ERROR: '");
  Serial.print (s);
  Serial.println ("'.");
}
  
