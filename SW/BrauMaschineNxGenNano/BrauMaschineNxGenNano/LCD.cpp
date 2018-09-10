#include "LCD.h"

#define FONT_H 10


LCD::LCD()
{
  u8g = new U8GLIB_PCD8544 (PIN_CLK, PIN_DIN, PIN_CE, PIN_DC, PIN_RST);  // CLK=8, DIN=4, CE=7, DC=5, RST=6
  // Set Backlight Intensity
  // 0 is Fully bright
  // 255 is off
  analogWrite(PIN_BACKLIGHT, 80);
  u8g->setFont(u8g_font_helvR08);  // select font
}

void LCD::prepare() {
}


void LCD::show( const char *c1, const char *c2, const char *c3, const char *c4) {
  u8g->firstPage();
  do {
    u8g->drawStr( 0, 8, c1);  // put string of display at position X, Y
    u8g->drawStr( 0, 18 ,c2);  // put string of display at position X, Y
    u8g->drawStr( 0, 28, c3);  // put string of display at position X, Y
    u8g->drawStr( 0, 38, c4);  // put string of display at position X, Y
    //u8g->drawStr( 0, 8, c);  // put string of display at position X, Y
    //u8g->drawStr(29, 35, "!");
  } while ( u8g->nextPage() );

}
