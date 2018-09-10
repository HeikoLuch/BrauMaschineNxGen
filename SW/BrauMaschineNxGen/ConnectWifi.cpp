
/**
   WifiConnection
*/
#include "Arduino.h"
#include "ConnectWifi.h"

#define EEPROM_POS_PWD 32
#define EEPROM_POS_SSID 0

ConnectWifi::ConnectWifi()
{
  wifiStrength = 0;
  ipString = "0.0.0.0";
}



// Return RSSI or 0 if target SSID not found
int32_t ConnectWifi::getStrength(const char* target_ssid) {
  byte available_networks = WiFi.scanNetworks();
  for (int network = 0; network < available_networks; network++) {
    String ssidNw = WiFi.SSID(network);

    if (ssidNw.compareTo(target_ssid) == 0) {
      return WiFi.RSSI(network);
    }
  }
  return 0;
}


/***********************************************
  Establishes connection to Wifi
  Read PWD and SSID from EEPROM.
  Using this credentials, trying to connect to Wifi.

*/

void ConnectWifi::checkConnection() {

  if (WiFi.status() == WL_CONNECTED) {

    return;
  }
  Serial.print("Trying to connect to "); Serial.println(ssid);

  // put your setup code here, to run once:

  readEEPROM(EEPROM_POS_SSID, 32, ssid);
  Serial.println (ssid);

  char pwd[32];
  readEEPROM(EEPROM_POS_PWD, 32, pwd);
  Serial.println (pwd);

  // attempt to connect to Wifi network:
  wifiStatus = WiFi.begin(ssid, pwd);

  byte cnt = 20;
  while ((WiFi.status() != WL_CONNECTED) && (--cnt > 0)) {
    delay(500);
    Serial.print(".");
  }

  /*
    if (wifiStatus != WL_CONNECTED) {
    Serial.print("Connection Failed with Status: ");
    if (wifiStatus == WL_NO_SHIELD) Serial.println("No WiFi shield is present");
    else if (wifiStatus == WL_IDLE_STATUS) Serial.println("");//: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
    else if (wifiStatus == WL_NO_SSID_AVAIL) Serial.println("No SSID are available");
    else if (wifiStatus == WL_SCAN_COMPLETED) Serial.println("The scan networks is completed");
    else if (wifiStatus == WL_CONNECT_FAILED) Serial.println("Connection fails for all the attempts");
    else if (wifiStatus == WL_CONNECTION_LOST) Serial.println("the connection is lost");
    else if (wifiStatus == WL_DISCONNECTED) Serial.println("Disconnected from a network");
    else Serial.println("Reason is unknown.");
    //return;
    }*/

  //Serial.println("Connected.");
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP:  ");
  ipString = WiFi.localIP().toString();
  Serial.println(ipString);
  Serial.print("Subnet: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Channel: ");
  Serial.println(WiFi.channel());
  Serial.print("Status: ");
  Serial.println(WiFi.status());
  Serial.print("WiFi stregth: ");  Serial.print(getStrength(ssid)); Serial.println("dBm");

}


// ---------------- private ----------------

void ConnectWifi::readEEPROM(int startAdr, int maxLength, char* dest) {
  EEPROM.begin(512);
  delay(10);
  for (int i = 0; i < maxLength; i++) {
    dest[i] = char(EEPROM.read(startAdr + i));
  }
  EEPROM.end();
}




