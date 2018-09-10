/**
   Web interface
*/
#include "Arduino.h"
#include "WebInterface.h"
#include "NTP-Clock.h"
//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define RAST_TEMPERATURE "_t"
#define RAST_TIME "_z"
#define RAST_HOLD "_h"

#define HTML_BR "<br>"

#define MAX_MENU_ENTRIES 4

// ================ VAR ===================

DeviceState *pDeviceState;
Memory *pMemory;
RunProgram *pRunProgram;
NTP* pNtp;

ESP8266WebServer server(80);

String URL;

// ================ CONST ===================

const String HTML_TABLE_HEADER_RASTEN = "<table style='border:1px solid;'><tr><th>Rast</th><th>Temperatur<br>[&deg;C]</th><th>Zeit<br>[min]</th><th>Anhalten</th></tr>";
const String HTML_BODY_END = "</body></html>";
const String HTML_TEXT_TYPE = "text/html";
const String HTML_MNU_LINK_RUN = "/run";

// ================ SUBMIT handler ========================

void handleSubmit() {

  //This is the call when submitting form: "GET /?0_t=22&0_z=20&0_r=50&0_h=on&1_t=24&1_z=20&1_r=10&2_t=26&2_z=15&2_r=10&3_t=26&3_z=20&3_r=10&4_t=26&4_z=30&4_r=10&5_t=28&5_z=30&5_r=10&5_h=on&6_t=30&6_z=20&6_r=10&6_h=on&cfg_simu=on HTTP/1.1"
#ifdef WEB_DEBUG

  for (int i = 0; i < server.headers(); i++) {
    Serial.print ("HeaderName: '"); Serial.print (server.headerName(i)); Serial.println("'");
    Serial.print ("Header: '"); Serial.print (server.header(i)); Serial.println("'");
  }
  for (int i = 0; i < server.args(); i++) {
    Serial.print ("Arg Name: '"); Serial.print (server.argName(i)); Serial.println("'");
    Serial.print ("Arg Value: '"); Serial.print (server.arg(i)); Serial.println("'");
  }
  Serial.print ("URI: '"); Serial.print (server.uri()); Serial.println("'");
#endif

  String argname;
  unsigned char argval;

  //if simulation isn't checked -> the argument is not sent; therefore set default
  pDeviceState->setSimulation (false);
  for (uint8_t z = 0; z < MAX_RASTEN;z++)
    pRunProgram->rasten[z].wait4User = false;
    
  for (uint8_t z = 0; z < server.args(); z++) {
    argname = server.argName(z);

    if (argname.equals("cfg_simu")) {
      pDeviceState->setSimulation(server.arg(z).equals("on"));
    }
    else {
      argval = (unsigned char) server.arg(z).toInt();

      String f = argname.substring(0, 1);
      //Serial.print("Zahl f:"); Serial.println(f);
      int i = f.toInt();  //rast number

      pRunProgram->rasten[i].wait4User = false;

      //Serial.print("Zahl i:"); Serial.println(i);

      if (argname.endsWith("_t"))
      {
        pRunProgram->rasten[i].temp = argval;
      }
      else if (argname.endsWith("_z"))
      {
        pRunProgram->rasten[i].time = argval;
      }
     
      else if (argname.endsWith("_h"))
      {
        if (server.arg(z).equals("on"))
         pRunProgram->rasten[i].wait4User = true;
      }
 #ifdef WEB_DEBUG
      Serial.print("===========");
#endif
    }
  }
}


// ================ PAGES per state =======================

/**********************************************************
      State: FINISHED
*/
String showPage_Finished() {
  String s = getHtmlFragment_HeadBody(0);
  s += getHtmlFragment_Title();
  s +=  getHtmlFragment_SubTitle("lightgreen", F("Brauen beendet."));


  String sa[] = {
    getHtml_MenuEntry ( F("KONFIGURIEREN"),  F("/setup")),
    "", "", ""
  };

  s += getHtmlFragment_Menu(sa) + "<br>";
  s += getHtmlFragment_ProtocolTable(false);

  s += get_progress_table();
  return s + HTML_BODY_END;
}

/**********************************************************
      State: RUNNING
*/
String showPage_Running() {

  String s = getHtmlFragment_HeadBody(5);
  s += getHtmlFragment_Title();
  s +=  getHtmlFragment_SubTitle(F("yellow"), F("Brauen gestartet!"));

  String sa[] = {
    
    getHtml_MenuEntry ( F("FORTSETZEN"),   F("/proceed"), pDeviceState->isRunning_Waiting()),
    getHtml_MenuEntry ( F("PAUSE"),  F("/pause")),
    getHtml_MenuEntry ( F("PROGRAMM ABBRECHEN"), F("/finish")),
    "", ""
  };
  s += getHtmlFragment_Menu(sa) + "<br>";
  s += getHtmlFragment_ProtocolTable(false);

  s += get_progress_table();
  return s + HTML_BODY_END;

}

/**********************************************************
      State: SETUP
*/
String showPage_Setup() {

  String s = getHtmlFragment_HeadBody(0);
  s += getHtmlFragment_Title();
  s +=  getHtmlFragment_SubTitle("lightgray", F("Hinweis: Rast eingeben oder Felder leer lassen."));

  String sa[] = {
    getHtml_MenuEntry ( "PROGRAMM STARTEN", HTML_MNU_LINK_RUN), "", "", ""
  };

  s += getHtmlFragment_Menu(sa);
  s += F("<br>");
  s += getHtml_TableForm();
  return s + HTML_BODY_END;
}


/**********************************************************
      State: PAUSING
*/
String showPage_Pause() {

  String s = getHtmlFragment_HeadBody(0);
  s += getHtmlFragment_Title();
  s +=  getHtmlFragment_SubTitle(F("lightpink"), F("Aktuelles Programm pausiert."));

  String sa[] = {
    getHtml_MenuEntry ( "PROGRAMM FORTSETZEN", HTML_MNU_LINK_RUN), "", "", ""
  };
  s += getHtmlFragment_Menu(sa);
  s += "<br>";
  s += getHtml_TableForm();
  return s + HTML_BODY_END;
}


// ================ HANDLER for Requests =========================

void _sendHtmlRequestMessage(String msg) {
  server.send ( 200, HTML_TEXT_TYPE, getHtmlFragment_HeadBody(3) + msg + HTML_BODY_END);
}

/*********************************
   Callback for RUN request
*/
void handleRun() {
  _sendHtmlRequestMessage (F("Starting / proceeding program!"));
  pDeviceState->requestToRun();
}


/*********************************
   Callback for Pause request
*/
void handlePause() {
  _sendHtmlRequestMessage (F("Handling pause request ..."));
  pDeviceState->requestToPause();
}

/*********************************
   Callback for Proceeding
*/
void handleProceed() {
  _sendHtmlRequestMessage (F("Handling proceed request ..."));
  pRunProgram->proceedRunRequested();
}

/*********************************
   Callback for FINISHED
*/
void handleFinish() {

  _sendHtmlRequestMessage (F("Finishing - please wait ..."));
  pDeviceState->requestToFinish();
}


/*********************************
   Callback for Stop request
*/
void handleSetup() {
  _sendHtmlRequestMessage (F("Going to setup ..."));
  pDeviceState->requestToSetup();
}


/*********************************
   Calback for ROOT request
*/
void handleRoot() {
  /*
    Serial.println ("--- Http ---");
    Serial.print ("URI: "); Serial.println (server.uri());
    Serial.print ("Argrument count: "); Serial.println (server.args());
  */

  // First: handle the parameters
  if ( server.args() > 0 ) {
    handleSubmit();
  }

  //show page with chage parameters
  if ( pDeviceState->isSetup()) {
    server.send ( 200, HTML_TEXT_TYPE, showPage_Setup() );
  }
  else if ( pDeviceState->isRunning()) {
    server.send ( 200, HTML_TEXT_TYPE, showPage_Running() );
  }
  else if (pDeviceState-> isFinished()) {
    server.send ( 200, HTML_TEXT_TYPE, showPage_Finished() );
  }
  else if (pDeviceState-> isPaused()) {
    server.send ( 200, HTML_TEXT_TYPE, showPage_Pause() );
  }
  else {
    server.send ( 200, HTML_TEXT_TYPE, "Unknown device state: " + String(pDeviceState->deviceState));
  }

}


/*********************************
   Callback on FAILED http request
*/
void handleNotFound() {
  String message = F("File Not Found\n\n");
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



// ================ PUBLIC functions =========================

/*************************************************
   INIT - call this in setup()
*/
void wsInit(DeviceState *d, Memory *m, String ip, NTP* n) {
  pNtp = n;
  pDeviceState = d;
  pMemory = m;
  Serial.println(ip);
  URL = "http://" + ip;
  Serial.println(URL);
  ESP8266WebServer *pServer = new ESP8266WebServer (80);
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on(HTML_MNU_LINK_RUN, handleRun);
  server.on("/pause", handlePause);
  server.on("/finish", handleFinish);
  server.on("/setup", handleSetup);
  server.on("/proceed", handleProceed);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}


/*************************************************
  LOOP - call this in loop()
**/
void wsLoop(RunProgram* rp) {
  pRunProgram = rp;
  server.handleClient();
}



// ========================== String Helpers ===========================

String getHtml_MenuEntry( String txt,  String link) {
  return getHtml_MenuEntry(txt, link, true);
}

String getHtml_MenuEntry( String txt,  String link, bool bClickable) {

  if (bClickable)  {
    String s = String ( "<a href='" + URL  + link + "'>" + txt + "</a>");
    //showString ("getHtml_MenuEntry()", String (condition) );
    return s;
  }
  return txt;
}

String getHtmlFragment_Menu (String p[])
{
  String s = F("<table cellpadding='10'><tr>");
  for (byte i = 0; i < MAX_MENU_ENTRIES; i++) {
    if (p[i].length() > 1 ) {
      s += F("<td>");
      s += p[i];
      s += F("</td>");
    }
  }
  s += "</tr></table>";
  return s;
}


String getHtmlFragment_HeadBody(int auto_refresh_sec) {
  String s = F("<!DOCTYPE HTML><html><head>");

  s += F("<TITLE>LUCH-Brauer</TITLE>");
  if (auto_refresh_sec > 0) {
    s += F("<meta http-equiv='refresh' content='");
    s += String(auto_refresh_sec);
    s += "; URL=" + URL + "'/>";
  }
  //JavaScript zum auf und zuklappen: http://alice-grafixx.de/JavaScript-Tutorial/Auf/Zu-klappen-159
  s += F("<script type='text/javascript'>function toggle(control){var elem = document.getElementById(control);if(elem.style.display == 'none'){elem.style.display = 'block';}else{elem.style.display = 'none';}}</script>");
  s += F("</head><body>\n");
  return s;
}




/*
  Show the parameters for 'Rasten' and make it editable.
*/
String form_input_make_table_row( RunProgram* runProgram, byte num) {
  //long startTime = millis();

  String INPUT_1 = F("<td align='center'><input type='text'  style='text-align: right;width: 48px;' name='");
  String INPUT_3 = F("' value= '");
  String INPUT_2 = F("' style='width: 50px;' align='right'>");

  //Column name
  String s = F("<tr><td>");
  s += (num + 1);
  s += F(": ");
  s += runProgram->getRast(num).name;
  s += F("</td>");

  //Column temperature
  s += INPUT_1;
  s += num;
  s += F(RAST_TEMPERATURE); // "_temp"
  s += INPUT_3;
  s += String(runProgram->getRast(num).temp);
  s += INPUT_2;
  s += F("</td>");

  //Column time
  s += INPUT_1;
  s += num;
  s += F(RAST_TIME);
  s += INPUT_3;
  s += String(runProgram->getRast(num).time);
  s += INPUT_2;
  s += F("</td>");
  /**
    //Column raise
    s += INPUT_1;
    s += num;
    s += F(RAST_RAISE);
    s += INPUT_3;
    s += String(rasten[num].raise);
    s += INPUT_2;
    s += F("</td>");
  */

  //Column hold
  s += F("<td><input type='checkbox' name='");
  s += num;
  s += F(RAST_HOLD);
  if (runProgram->getRast(num).wait4User)
    s += F("' checked >") ;
  else
    s += F("'>") ;
  s += F("</td></tr>");

  return s;

  //Serial.print ("Laufzeit form_input_make_table_row():");
  //Serial.println(millis() - startTime);
}



/**
  Calculate HTML String containing table showing the values of parameters
*/
String send_parameter_table (RunProgram wsRunProgram) {
  //long startTime = millis();


  //calculate Color
  String bgColorDone = F(" bgcolor=#32CD32");
  String bgColorInWork = F(" bgcolor=#FF8C00");
  String bgColor = "";

  String s = HTML_TABLE_HEADER_RASTEN;
  //loop over parameters
  for ( byte num = 0; num < MAX_RASTEN; num++) {

    //Column name
    bgColor = "";
    if (num == pRunProgram->getCurrentRastNum())
      bgColor = bgColorInWork;
    else if (num < pRunProgram->getCurrentRastNum())
      bgColor = bgColorDone;

    s += "<tr><td";
    s += bgColor + ">";
    s += String (num + 1);
    s += ": ";
    s += pRunProgram->getRast(num).name;
    s += "</td><td";

    //Column temperature
    bgColor = "";
    if (num < pRunProgram->getCurrentRastNum())
      bgColor = bgColorDone;

    else if (num == pRunProgram->getCurrentRastNum()) {
      bgColor = bgColorInWork;
      if (pRunProgram->getProtocol().timeRastStart[num] > 0) //Heizende erreicht -> rast begonnen.
        bgColor = bgColorDone;
    }

    s += bgColor + ">";
    s += String(pRunProgram->getRast(num).temp);
    s += "</td><td";


    //Column time
    bgColor = "";
    if (num < pRunProgram->getCurrentRastNum())
      bgColor = bgColorDone;
    else if (num == pRunProgram->getCurrentRastNum()) {
      bgColor = bgColorInWork;
      if (pRunProgram->getProtocol().timeRastStart[num] == 0) //Heizende noch nicht erreicht.
        bgColor = "";
      //else if (rast_halt_executed)
      //  bgColor = bgColorDone;
    }

    s += bgColor + ">";

    s +=  String(pRunProgram->getRast(num).time);
    s += "</td><td";

    /*
      //Column raise
      s2C( F(">"));
      s2C( String(rasten[num].raise));
      s2C( F("</td><td"));
    */

    //Column hold
    bgColor = "";
    if (num < pRunProgram->getCurrentRastNum())
      bgColor = bgColorDone;
    //    else if ((num == getCurrentRastNum()) && rast_halt_executed)
    //      bgColor = bgColorInWork;

    s += bgColor + F(">");
    if (pRunProgram->getRast(num).wait4User)
      s += F("JA");
    else
      s += F("NEIN") ;
    s += F("</td>");
  }
  s +=  F("</table>");
  return s;
}

/*
  String getTimeString() {
  return "keine Zeit";//ntp_time_string();
  }*/

String getHtml_TableForm()
{
  String s = "<form method=get>";
  s += HTML_TABLE_HEADER_RASTEN;

  //Because of memory issues the table each row is transmitted in one package
  for ( byte num = 0; num < MAX_RASTEN; num++) {
    s += form_input_make_table_row (pRunProgram,  num); //show HTML-Inputs
  }

  s += "</table><p>Brauprozess simulieren?";
  s += "<input type='checkbox' name='cfg_simu'";
  if (pDeviceState->isSimulating())
    s += "checked";
  s += "></p><p>";
  s += "<input type='submit' value='Konfiguration anwenden' align='right'></p></form>";
  return s;
}

String getHtmlFragment_ProtocolTable (byte expanded ) {
  //long startTime = millis();
  String s = createJsToggle( F("tblProto"), F("Brau-Protokoll"));
  s += "<div id='tblProto' style='display: ";
  if (expanded == 0)
    s += "none'>";
  else
    s += "block'>";
  s += "<p>Programmstart: ";
  s += pNtp->time2TimeString(pRunProgram->getProtocol().timeStart);

  s += ", Laufzeit: ";
  s += pNtp->time2TimeString(pRunProgram->getProtocol().timeStop - pRunProgram->getProtocol().timeStart);
  s += "<br>";
  s += "<table><tr><th>Brauschritt</th><th>Startzeit</th><th>Rastbeginn</th>";
  for ( byte num = 0; num < MAX_RASTEN; num++) {
    s += "<tr><td>";
    s += pRunProgram->getRast(num).name;
    s += "</td><td>";
    s += pNtp->time2TimeString(pRunProgram->getProtocol().timeHeatingStart[num]);
    s += "</td><td>";
    s += pNtp->time2TimeString(pRunProgram->getProtocol().timeRastStart[num]);
    s += "</td></tr>";
  }
  s += "</table></p></div>";
  return s;
}


String createJsToggle(String id, String linkText) {
  String s =  F("<a href=\"javascript:toggle('");
  s += id;
  s += F("')\">");
  s += linkText;
  s += F("</a>");
  return s;
}


/*************************************
   Show progress when running
*/
String get_progress_table () {
  //long startTime = millis();

  //calculate Color
  String bgColorDone = F(" bgcolor=#32CD32");
  String bgColorInWork = F(" bgcolor=#FF8C00");
  String bgColor = "";

  String s = HTML_TABLE_HEADER_RASTEN;

  byte rast_num_current = pRunProgram->getCurrentRastNum();
  protocolType protocol = pRunProgram->getProtocol();
  //bool rast_halt_executed = pRunProgram->isRastHaltActive();

  //loop over parameters
  for ( byte num = 0; num < MAX_RASTEN; num++) {

    //Column name
    bgColor = "";
    if (num == rast_num_current)
      bgColor = bgColorInWork;
    else if (num < rast_num_current)
      bgColor = bgColorDone;

    s += "<tr><td";
    s += bgColor + ">";
    s += String (num + 1);
    s += ": ";
    s += pRunProgram->getRast(num).name;
    s += "</td><td";

    //Column temperature
    bgColor = "";
    if (num < rast_num_current)
      bgColor = bgColorDone;
    else if (num == rast_num_current) {
      bgColor = bgColorInWork;
      if ( protocol.timeRastStart[num] > 0) //Heizende erreicht -> rast begonnen.
        bgColor = bgColorDone;
    }

    s += bgColor + ">";
    s += String(pRunProgram->getRast(num).temp);
    s += "</td><td";


    //Column time
    bgColor = "";
    if (num < rast_num_current)
      bgColor = bgColorDone;
    else if (num == rast_num_current) {
      bgColor = bgColorInWork;
      if (protocol.timeRastStart[num] == 0) //Heizende noch nicht erreicht.
        bgColor = "";
      else if (pDeviceState->isRunning_Waiting())
        bgColor = bgColorDone;
    }

    s += bgColor + ">";

    s += String(pRunProgram->getRast(num).time);
    s += "</td><td";

    /*
      //Column raise
      s2C( F(">"));
      s2C( String(rasten[num].raise));
      s2C( F("</td><td"));
    */

    //Column hold
    bgColor = "";
    if (num < rast_num_current)
      bgColor = bgColorDone;
    else if ((num == rast_num_current) && pDeviceState->isRunning_Waiting())
      bgColor = bgColorInWork;

    s +=  bgColor + F(">");
    if (pRunProgram->getRast(num).wait4User)
      s +=  F("JA");
    else
      s +=  F("NEIN");
    s +=  F("</td>");
  }
  s +=  F("</table>");

  return s;
}


String getHtmlFragment_Title()
{
  return "<h3><a href='" + URL + "'>LUCH-Brau-Maschine</a></h3>";
}

String getHtmlFragment_SubTitle(String bgColor, String hint) {
  String s = "<p><div style='background-color:" + bgColor + ";'><i>" + hint + "<br><small>";
  //s += F("keine Zeit");//ntp_time_string();
  s += pNtp->getDateTimeString();
  s += F("</small></div></i></p>");
  return s;
}



