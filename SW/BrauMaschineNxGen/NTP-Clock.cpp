#include "Arduino.h"
#include "NTP-Clock.h"
#include <TimeLib.h>
#include <ESP8266WiFi.h>


//#define DEBUG_NTP



/*****************************************
    int h 0,1,2... Number of hours used as offset
*/
NTP::NTP(int h) {
  Serial.println(F("Starting UDP"));
  udp.begin(localPort);
  Serial.print(F("Local port: "));
  Serial.println(udp.localPort());

  hoursOffset = 3600 * h;
}



/*****************************************
   Call this method within loop
*/
void NTP::loop() {
  if ((millis() - timeNtpResponse) < NTP_UPDATE_PERIODE)
    return;

  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println(F("NTP Error: no packet yet."));
  }
  else {
    timeNtpResponse = millis();
#ifdef DEBUG_NTP
    Serial.print(F("NTP synced; packet received, length="));
    Serial.println(cb);
#endif    
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = " );
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time(Unix time); Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = hoursOffset + secsSince1900 - seventyYears;
    setTime(epoch); //sync TimeLib.h with Unix Time

    #ifdef DEBUG_NTP
    Serial.println ( getDateTimeString());
    #endif
  }
}


/*****************************************

*/
// send an NTP request to the time server at the given address
unsigned long NTP::sendNTPpacket(IPAddress& address)
{
  #ifdef DEBUG_NTP
  Serial.println("sending NTP packet...");
  #endif
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}


/*****************************************
   Convert INT to String and fill leading '0' if necessary
*/
String NTP::convert2String(int i) {
  String s = "";
  s += i;
  if (s.length() == 1)
    return "0" + s;
  return s;
}


/*****************************************
  Returns hh:mm:ss
*/
String NTP::time2TimeString(time_t t) {
  // digital clock display of the time

  String s = F("");
  s += convert2String (hour(t));
  s += F(":");
  s += convert2String (minute(t));
  s += F(":");
  s += convert2String (second(t));
  return s;
}

/*****************************************
  Returns a complete date & time string
*/
String NTP::getDateTimeString() {
  return time2DateString(now()) + " " + time2TimeString(now());
}


/*****************************************
  Returns weekday as string for weekday(time);
*/
String NTP::getWeekDayAsString(int i) {
  switch (i) {
    case 1: return String(F("Sunday"));
    case 2: return String(F("Monday"));
    case 3: return String(F("Tuesday"));
    case 4: return String(F("Wednesday"));
    case 5: return String(F("Thursday"));
    case 6: return String(F("Friday"));
    case 7: return String(F("Saturday"));
    default: return convert2String(i);
  }
}


/*****************************************
  Returns dd.MM.yyyy
*/
String NTP::time2DateString(time_t t) {
  // digital clock display of the time

  String s = getWeekDayAsString (weekday(t));
  s += F(", ");
  s += convert2String (day(t));
  s += F(".");
  s += convert2String (month(t));
  s += F(".");
  s += convert2String (year(t));
  return s;
}

