#include "Arduino.h"
#include <WiFiUdp.h>

#ifndef NTP_CLOCK_h
#define NTP_CLOCK_h

/*************************************************************************
 * This class provides real time information based on NTP server.
 * It uses the TimeLib.h for convenience (i.e. string representation).
 * 
 * The NTP server is contaced on init and then periodically to keep the time in TimeLib synced.
 * The lib is based on the ideas from https://www.geekstips.com/arduino-time-sync-ntp-server-esp8266-udp/.
 * The class provides time string as string.
 */
// NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

 //how often shall the server be updated? in ms
#define NTP_UPDATE_PERIODE 60000L

class NTP {

  private:
  
    unsigned long timeNtpResponse; //the time since last  ntp server response
    unsigned long hoursOffset; //hours offset in seconds, in context of timezone
    
    IPAddress timeServerIP; // time.nist.gov NTP server address
    const char* ntpServerName = "time.nist.gov";
    unsigned int localPort = 2390;      // local port to listen for UDP packets

    byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

    // A UDP instance to let us send and receive packets over UDP
    WiFiUDP udp;

    unsigned long sendNTPpacket(IPAddress& address);

    String convert2String(int i);
    String getWeekDayAsString(int i);
  
  public:
    NTP(int hours);
    void loop();
    String time2TimeString(time_t t);
    String time2DateString(time_t t);
    
    String getDateTimeString();
    
};


#endif
