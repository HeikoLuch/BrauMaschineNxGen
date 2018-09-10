#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#ifndef conn_wifi_h
#define conn_wifi_h

//#define DEBUG_CONNWIFI


/**************************************************************
* This class handles the WiFi connection-
* 	- SSID and password are read from EEPROM
*
*
*/


//how oft shall the time consuming method getRSSI() be called?
#define UPDATE_STRENGTH_MINUTE 3 


class ConnectWifi {
	
private:
	char ssid[32];
	int wifiStatus;
	
	int32_t wifiStrength;
	unsigned long wifiStrengthInterval;

	void readEEPROM(int startAdr, int maxLength, char* dest);
	
	
public:
  //the IP as string
  String ipString;
  
 	ConnectWifi();
 	
 	// establish connection if status is NOT CONNECTED
 	void checkConnection();
 	
 	// return strength in dBm; Takes appx 1500 ms!
	// Return RSSI or 0 if target SSID not found
	int32_t getStrength(const char* target_ssid);
 
};



#endif
