#include "Arduino.h"
#include "U8glib.h"

#ifndef lcd_h
#define lcd_h

#define PIN_RST 12
#define PIN_CE 11
#define PIN_DC 10
#define PIN_DIN 9
#define PIN_CLK 8
#define PIN_BACKLIGHT A1

//U8GLIB_PCD8544 u8g(8, 9, 11, 10, 12);    // SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9, Reset = 8


class LCD {
	
private:
	char c1[20];
  // Nokia 5110 Display
  //#define backlight 11
 
  //Delcare the display and assign the pins
  // see https://github.com/olikraus/u8glib/wiki/fontsize
  U8GLIB_PCD8544* u8g;
  
public:
	
	LCD();
	void prepare(); 
  void show(const char *, const char *, const char *, const char *); //print some infos regarding memory state
};



#endif
