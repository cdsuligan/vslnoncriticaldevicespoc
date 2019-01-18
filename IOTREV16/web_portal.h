#ifndef WEB_PORTAL_H
#define WEB_PORTAL_H

#include <ESP8266WebServer.h>
#include "EEPROMAnything.h"
#include "setpins.h"



struct Reporting {
  int frequency;
  bool D0State;
  char D0Name[15];
  bool D1State;
  char D1Name[15];  
  bool D2State;
  char D2Name[15];
  char A0State;
  char A0Name[15];
  int A0Scale;
  int A0Threshold;
  char messageID[15];
};

    
void handleSaveDweet ();

void start_server();

void stop_server();

void handle_client();

void ResetESP();

void FactorySettings();

void handleRoot();

void handleSettings();

void handlePinSet();

void handleReporting();

void handleSet();

void handleSaveReport();

void handleConKey();

void handleSaveConKey();

void handleSaveResponse();

#endif /* WEB_PORTAL_H */
