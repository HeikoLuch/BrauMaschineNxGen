#include "Arduino.h"
#include "DeviceState.h"
#include "Memory.h"
#include "RunProgram.h"
#include "NTP-Clock.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#ifndef web_h
#define web_h

#define WEB_DEBUG
/*****************************************************************
 * Introduction
 * This class provides all web pages being necessary to control LuchBrewer.
 * 
 * The webServer is working as described here:  https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/HelloServer/HelloServer.ino
 * 
 * A http request (root, run, pause etc.) is handled using the functions handleXXX().
 * 
 * The root page shows the HTML page, depending on current device state.
 * Each device state generates a separate HTML page - see showPage_xxx().
 * 
 * Each HTML page consists of
 * - Header & Body begin
 * - Title
 * - Subtitle
 * - Menus (to trigger other device states)
 * - Content (Table form or protocol tables, ...)
 * - Body end
 */

// ------ 'public' ------

void wsInit (DeviceState *d, Memory *m, String ip, NTP* ntp);
void wsLoop( RunProgram* runProgram);

// ------ 'private' ------

//--- show Pages based on current device state ---
String showPage_Running();
String showPage_Finished();
String showPage_Setup();
String showPage_Pause();

// --- reusable fragments applied in showPage_xxx() ---
String getHtmlFragment_Menu (String p[]);

String getHtmlFragment_Title();
String getHtmlFragment_SubTitle(String bgColor, String hint);
String getHtmlFragment_ProtocolTable (byte expanded );
String getHtmlFragment_HeadBody(int auto_refresh_sec);  //Automatically refresh the page if auto_refresh_sec > 0.
//String getTimeString();

// other HTML string helper 
String getHtml_MenuEntry( String txt,  String link);
String getHtml_MenuEntry( String txt,  String link, bool bClickable);

String createJsToggle(String id, String linkText);
String get_progress_table ();


void handleSubmit();

// 

// create the table for entering program parameters
String getHtml_TableForm();
String form_input_make_table_row( RunProgram runProgram, byte num);
#endif
