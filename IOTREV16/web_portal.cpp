#include "web_portal.h"

ESP8266WebServer server ( 80 );

const char* www_username = "admin";
const char* www_password = "admin";


const char HTTP_HEADER[] PROGMEM = R"=====(  
    <!DOCTYPE html>
    <html lang=en>
    <head>
    <TITLE>ESP8266 Server</TITLE>
    <meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <style> body { background-color: #223A5E; font-family: Arial, Helvetica, Sans-Serif; Color: #FFFFFF; }</style>
    <style> a:link{color: white;} a:visited{color: white;} a:hover{color: hotpink;} a:active{color: orange;}</style>
    <style> .button { background-color: #4CAF50; border: on; color: white; padding: 6px 25px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer;}</style>
    <style> #myModal { color:green; font-size: 20px; background-color: white; border: 2px solid green; position: absolute; top: 40%; left: 50%; -webkit-transform: translate(-50%, -50%); transform: translate(-50%, -50%); height: 110px; width: 250px;}</style>
    <style> .cont{ margin-left: 5%;}</style>
    )=====";

const char HTTP_FIELDSET_PARAM[] PROGMEM  = "<fieldset id='{f_id)' style='{s_d)'><legend>{L)</legend>{i1){i2){i3){i4){i5){i6)</fieldset>";
										
const char HTTP_INPUT_PARAM[] PROGMEM  =	"<input type='{i_t)'name='{N)'id='{id}'value='{V)'{pf)>{F)";								

const char HTTP_HEADER_PARAM[] PROGMEM  = "<body><h1>{h1}</h1><h2>{h2}</h2><h3>{h3}</h3><h4>{h4}</h4><form id='{f_i)'>";	

/*
   server.setContentLength(sizeof(HTTP_HEADER) + sizeof(SETTINGS_SAVED)); //make static length if known to be constant
   server.send ( 200, "text/html", HTTP_HEADER ); 
   server.sendContent(SETTINGS_SAVED);
*/ 



void start_server(){
  
    server.on ("/", handleRoot);
    server.on("/PinSetReporting", handlePinSetReporting);
    server.on ("/setReporting", handleSaveReport);
    server.on ("/ResetESP", ResetESP);
    server.on ("/FactorySettings", FactorySettings); 
    server.on ("/Settings", handleSettings);
    server.on ("/ConKey", handleConKey);
    server.on ("/SetAwsKey", handleSaveAwsEndKey);
    
    server.on ("/SetAwsCon", handleSaveAwsConKey);
    server.on ("/SetAwsSec", handleSaveAwsSecKey);
    server.on ("/SetGcpDev", handleSaveGcpDev);
    server.on ("/SetGcpReg", handleSaveGcpReg);
    
    server.on ("/SetConKey", handleSaveConKey);    
    server.on ("/SetDweet", handleSaveDweet);
    server.on ("/SetPlatform", savePlatformState);
    
    server.begin();
}




void handle_client(){
    server.handleClient();
}




void handleRoot()
{
      if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
      }
    
      String Page = FPSTR(HTTP_HEADER);
      Page +=  F("<body>"
      "<h1 align=\"center\">Eff% IoT</h1>"
      "<h3>Please select a button below</h3>"
      "<a href=PinSetReporting>Input Analytics</a>"
      "</br>"
      "<a href=ConKey>IOT Connection Key</a>"
      "</br>"
      "<a href=Settings>Settings</a>"
      "</body>"
      "</html>"); 
      server.sendHeader("Content-Length", String(Page.length())); 
      server.send ( 200, "text/html", Page );  
}




void handleSaveReport() {

     struct Reporting PinReporting;

     if (!server.authenticate(www_username, www_password)) {
       return server.requestAuthentication();
     }
    
       if(server.arg("D0State") == "D")
     {
       PinReporting.D0State = true;
     }
       else if(server.arg("D0State") == "O")
     {
       PinReporting.D0State = false;
     }
    
       if(server.arg("D1State") == "D")
     {
       PinReporting.D1State = true;
     }
       else if(server.arg("D1State") == "O")
     {
       PinReporting.D1State = false;
     }
    
       if(server.arg("D2State") == "D")
     {
       PinReporting.D2State = true;
     }
       else if(server.arg("D2State") == "O")
     {
       PinReporting.D2State = false;
     }
    
       if(server.arg("A0State") == "A")
     {
       PinReporting.A0State = 2;
     }
       else if(server.arg("A0State") == "D")
     {
       PinReporting.A0State = 1;
     }
       else if(server.arg("A0State") == "O")
     {
       PinReporting.A0State = false;
     }
     PinReporting.A0Scale = server.arg("A0Scale").toInt(); //consider changing these to sprintf
     PinReporting.frequency = server.arg("frequency").toInt();
     PinReporting.A0Threshold = server.arg("A0Threshold").toInt();
     strcpy(PinReporting.D0Name, server.arg("D0Name").c_str());
     PinReporting.D0Name[server.arg("D0Name").length()] = '\0'; 
     strcpy(PinReporting.D1Name, server.arg("D1Name").c_str());
     PinReporting.D1Name[server.arg("D1Name").length()] = '\0'; 
     strcpy(PinReporting.D2Name, server.arg("D2Name").c_str());
     PinReporting.D2Name[server.arg("D2Name").length()] = '\0'; 
     strcpy(PinReporting.A0Name ,server.arg("A0Name").c_str());
     PinReporting.A0Name[server.arg("A0Name").length()] = '\0'; 
     strcpy(PinReporting.messageID ,server.arg("messageID").c_str());
     PinReporting.messageID[server.arg("messageID").length()] = '\0'; 
     EEPROM_writeAnything(0, PinReporting); 
}




void ResetESP() {

    if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }
    ESP.restart();
}




void FactorySettings() {
  
    if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }
    EEPROM_writeAnything(155, "NO");
    ESP.restart();
}




void handleSaveConKey() {

   if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
   }
   String s = server.arg("url_encode");
   char ConKey[190]; 
   strcpy(ConKey, s.c_str());
   ConKey[s.length()] = '\0';//add the null terminator at the end of the string
   EEPROM_writeAnything(163, ConKey);
  
}


////////////////////////////////////////////////////////////
void handleSaveAwsEndKey(){
  if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
  }
  String d = server.arg("url_encode1");
  char AWSEnd[55];
  strcpy(AWSEnd, d.c_str());
  AWSEnd[d.length()] = '\0';
  EEPROM_writeAnything(353, AWSEnd);
}



void handleSaveAwsConKey(){
  if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
  }
  String f = server.arg("url_encode2");
  char AWSConKey[25];
  strcpy(AWSConKey, f.c_str());
  AWSConKey[f.length()] = '\0';
  EEPROM_writeAnything(408, AWSConKey);
}



void handleSaveAwsSecKey(){
  if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
  }
  String g = server.arg("url_encode3");
  char AWSSecKey[45];
  strcpy(AWSSecKey, g.c_str());
  AWSSecKey[g.length()] = '\0';
  EEPROM_writeAnything(433, AWSSecKey);
}



void handleSaveGcpDev(){
  if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
  }
  String h = server.arg("url_encode4");
  char GCPDevice[15];
  strcpy(GCPDevice, h.c_str());
  GCPDevice[h.length()] = '\0';
  EEPROM_writeAnything(478, GCPDevice);
}



void handleSaveGcpReg(){
  if (!server.authenticate(www_username, www_password)) {
   return server.requestAuthentication();
  }
  String j = server.arg("url_encode5");
  char GCPRegistry[15];
  strcpy(GCPRegistry, j.c_str());
  GCPRegistry[j.length()] = '\0';
  EEPROM_writeAnything(495, GCPRegistry);
}

////////////////////////////////////////////////////////////


void handleSaveDweet() {

    if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }
    bool DweetData;
    if(server.arg("IPState") == "1"){
    DweetData = true;
    }
    else {DweetData = false;}
    EEPROM_writeAnything(163, DweetData);
}




void savePlatformState(){
  
    if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }
    int platformNo;

    if(server.arg("PlatformState")=="1"){    
    platformNo = 1; 
    }
    else if(server.arg("PlatformState")=="2"){
    platformNo = 2; 
    }
    else if(server.arg("PlatformState")=="3"){
    platformNo = 3; 
    }
    EEPROM_writeAnything(140, platformNo);
}


    

void handlePinSetReporting(){

    if(!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }  
    struct ESPPins ReadPinSet;
    EEPROM_readAnything(128,ReadPinSet);
    
    struct Reporting PinReporting;
    EEPROM_readAnything(0,PinReporting);

      String Page = FPSTR(HTTP_HEADER);
    Page += FPSTR(HTTP_HEADER_PARAM);
    Page.replace("{h1}", "Azure Setup");
    Page.replace("{h2}", "");
    Page.replace("{h3}", "");
    Page.replace("{h4}", "");
    Page.replace("{f_i)", "Pinform");
  
      String item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{F)", "");
    item.replace("{L)", "Digital Input 0");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D0State");
    item.replace("{id}", "D0State");
    item.replace("{V)", "D");
    if(PinReporting.D0State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "ON");
    item.replace("{i2)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D0State");
    item.replace("{id}", "D0State");
    item.replace("{V)", "O");
    if(!PinReporting.D0State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "OFF ");
    item.replace("{i3)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "text");
    item.replace("{N)", "D0Name");
    item.replace("{id}", "D0Name");
    item.replace("{V)", PinReporting.D0Name);   
    item.replace("{pf)", "size=4 autofocus");
    item.replace("{F)", "");  
    item.replace("{i4)", ""); 
    item.replace("{i5)", ""); 
    item.replace("{i6)", "");   
    Page += item;

    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Digital Input 1");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D1State");
    item.replace("{id}", "D1State");
    item.replace("{V)", "D");
    if(PinReporting.D1State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "ON");
    item.replace("{i2)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D1State");
    item.replace("{id}", "D1State");
    item.replace("{V)", "O");
    if(!PinReporting.D1State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "OFF ");
    item.replace("{i3)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "text");
    item.replace("{N)", "D1Name");
    item.replace("{id}", "D1Name");
    item.replace("{V)", PinReporting.D1Name);   
    item.replace("{pf)", "size=4 autofocus");
    item.replace("{F)", "");  
    item.replace("{i4)", ""); 
    item.replace("{i5)", ""); 
    item.replace("{i6)", "");   
    Page += item;
  
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Digital Input 2");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D2State");
    item.replace("{id}", "D2State");
    item.replace("{V)", "D");
    if(PinReporting.D2State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "ON");
    item.replace("{i2)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "radio");
    item.replace("{N)", "D2State");
    item.replace("{id}", "D2State");
    item.replace("{V)", "O");
    if(!PinReporting.D1State){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "OFF ");
    item.replace("{i3)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "text");
    item.replace("{N)", "D2Name");
    item.replace("{id}", "D2Name");
    item.replace("{V)", PinReporting.D2Name);   
    item.replace("{pf)", "size=4 autofocus");
    item.replace("{F)", "");  
    item.replace("{i4)", ""); 
    item.replace("{i5)", ""); 
    item.replace("{i6)", "");   
    Page += item; 
   
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Analogue Input 0");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "A0State");
    item.replace("{id}", "A0State");
    item.replace("{V)", "A");
    if(PinReporting.A0State == 2){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "AC");
    item.replace("{i2)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "radio");
    item.replace("{N)", "A0State");
    item.replace("{id}", "A0State");
    item.replace("{V)", "D");
    if(PinReporting.A0State == 1){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "DC");
    item.replace("{i3)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "radio");
    item.replace("{N)", "A0State");
    item.replace("{id}", "A0State");
    item.replace("{V)", "O");
    if(PinReporting.A0State == false){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "OFF &nbsp &nbsp");
    item.replace("{i4)", HTTP_INPUT_PARAM);   
    item.replace("{i_t)", "text");
    item.replace("{N)", "A0Name");
    item.replace("{id}", "A0Name");
    item.replace("{V)", PinReporting.A0Name);
    item.replace("{pf)", "size=4 autofocus");
    item.replace("{F)", "&nbsp &nbsp SCALE &nbsp &nbsp"); 
    item.replace("{i5)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "text");
    item.replace("{N)", "A0Scale");
    item.replace("{id}", "A0Scale");
    item.replace("{V)", String(PinReporting.A0Scale));
    item.replace("{pf)", "size=1 autofocus");
    item.replace("{F)", "&nbsp &nbsp THRESHOLD &nbsp &nbsp"); 
    item.replace("{i6)", HTTP_INPUT_PARAM); 
    item.replace("{i_t)", "text");
    item.replace("{N)", "A0Threshold");
    item.replace("{id}", "A0Threshold");
    item.replace("{V)", String(PinReporting.A0Threshold));
    item.replace("{pf)", "size=1 autofocus");
    item.replace("{F)", "");    
    Page += item;
  
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Pin Polling Frequency");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "text");
    item.replace("{N)", "frequency");
    item.replace("{id}", "frequency");
    item.replace("{V)", String(PinReporting.frequency));
    item.replace("{pf)", "size=1 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", ""); 
    item.replace("{i3)", "");   
    item.replace("{i4)", ""); 
    item.replace("{i5)", ""); 
    item.replace("{i6)", "");   
    Page += item;
    
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Message Count ID");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "text");
    item.replace("{N)", "messageID");
    item.replace("{id}", "messageID");
    item.replace("{V)", PinReporting.messageID);
    item.replace("{pf)", "size=4 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", ""); 
    item.replace("{i3)", "");   
    item.replace("{i4)", ""); 
    item.replace("{i5)", ""); 
    item.replace("{i6)", "");   
    Page += item; 
  
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Actions");
    item.replace("{i1)", "<div>");
    item.replace("{i2)", "<br>");
    item.replace("{i3)", "<button id=\"set_button\" class=\"button\">Set</button>");
    item.replace("{i4)", "</div>");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;

    Page += F("<div class=\"modal\" id=\"myModal\" style=\"display: none;\"><div class=\"modal-dialog\">"
              "<div class=\"modal-content\"><!-- Modal Header --><div class=\"modal-header cont\"><h4 class=\"modal-title\">Changes have been set</h4></div>"
              "<div class=\"modal-footer cont\">"
              "<button type=\"button\" class=\"btn btn-danger\" data-dismiss=\"modal\" id=\"closeBtn\">Confirm</button></div></div></div></div>"
              "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script><script>"
              "var D0State;var D1State;var D2State;var A0State;var D0Name;var D1Name;var D2Name;var A0Name;var frequency; var A0Scale; var messageID; var A0Threshold; $('#set_button').click(function(OnEvent){ OnEvent.preventDefault();"
              "D0State = $('input[name=D0State]:checked', '#Pinform').val(); D1State = $('input[name=D1State]:checked', '#Pinform').val(); D2State = $('input[name=D2State]:checked', '#Pinform').val(); A0State = $('input[name=A0State]:checked', '#Pinform').val(); "
              "D0Name = $('#D0Name').val(); D1Name = $('#D1Name').val(); D2Name = $('#D2Name').val(); A0Name = $('#A0Name').val(); frequency = $('#frequency').val(); A0Scale = $('#A0Scale').val(); messageID = $('#messageID').val(); A0Threshold = $('#A0Threshold').val();"
              "$.get('/setReporting?D0State=' + D0State + '&D1State=' + D1State + '&D2State=' + D2State + '&A0State=' + A0State + '&D0Name=' + D0Name + '&D1Name=' + D1Name + '&D2Name=' + D2Name + '&A0Name=' + A0Name + '&frequency=' + frequency + '&A0Scale=' + A0Scale + '&messageID=' + messageID + '&A0Threshold=' + A0Threshold, function(PinState){ console.log(PinState);});"
              "$('#myModal').show();});"
              "$('#closeBtn').click(function(OnEvent){ $('#myModal').hide();});"
              "</script>"
              "</body>"
              "</html>");

              server.send ( 200, "text/html", Page);    
}




void handleConKey() {
  
    if(!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
    }
  
    char ConKey[190];
    EEPROM_readAnything(163, ConKey);

    char AWSEnd[55];
    EEPROM_readAnything(353, AWSEnd);
    
    char AWSConKey[25];
    EEPROM_readAnything(408, AWSConKey);
    
    char AWSSecKey[45];
    EEPROM_readAnything(433, AWSSecKey);

    char GCPDevice[15];
    EEPROM_readAnything(478, GCPDevice);
    
    char GCPRegistry[15];
    EEPROM_readAnything(495, GCPRegistry);

    int PlatformData;
    EEPROM_readAnything(140,PlatformData);

      String Page = FPSTR(HTTP_HEADER);
  	Page += FPSTR(HTTP_HEADER_PARAM);
  	Page.replace("{h1}", "Azure IOT Central Connection Key");
  	Page.replace("{h2}", "Paste your connection key below");
  	Page.replace("{h3}", "");
  	Page.replace("{h4}", "");
  	Page.replace("{f_i)", "ConKeyForm");
  
    //platform radio buttons
    String item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "Platform");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "PlatformState");
    item.replace("{id}", "PlatformState1");
    item.replace("{V)", "1");
    if(PlatformData == 1){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "IoT Central");
    item.replace("{i2)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "PlatformState");
    item.replace("{id}", "PlatformState2");
    item.replace("{V)", "2");
    if(PlatformData == 2){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "AWS");
    item.replace("{i3)", HTTP_INPUT_PARAM);
    item.replace("{i_t)", "radio");
    item.replace("{N)", "PlatformState");
    item.replace("{id}", "PlatformState3");
    item.replace("{V)", "3");
    if(PlatformData == 3){item.replace("{pf)", "checked");}
    else{item.replace("{pf)", "");}
    item.replace("{F)", "GCP");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;
  	
  	item = FPSTR(HTTP_FIELDSET_PARAM);
  	item.replace("{L)", "Azure IoT Central Connection Key");
  	item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "Azure");
    if(PlatformData != 1){item.replace("{s_d)", "display:none");}
  	item.replace("{i_t)", "text1");
  	item.replace("{N)", "ConKey");
  	item.replace("{id}", "ConKey");
  	item.replace("{V)", ConKey);
  	item.replace("{pf)", "size=30 autofocus");
  	item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
  	Page += item;

    //////////////////////////////////////////////////////////////////////////////////////
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "AWS Endpoint");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "AWS1");
    if(PlatformData != 2){item.replace("{s_d)", "display:none");}
    item.replace("{i_t)", "text");
    item.replace("{N)", "AWSEnd");
    item.replace("{id}", "AWSEnd");
    item.replace("{V)", AWSEnd);
    item.replace("{pf)", "size=30 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;

    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "AWS Connection Key");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "AWS2");
    if(PlatformData != 2){item.replace("{s_d)", "display:none");}
    item.replace("{i_t)", "text");
    item.replace("{N)", "AWSConKey");
    item.replace("{id}", "AWSConKey");
    item.replace("{V)", AWSConKey);
    item.replace("{pf)", "size=30 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;

    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "AWS Secret Key");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "AWS3");
    if(PlatformData != 2){item.replace("{s_d)", "display:none");}
    item.replace("{i_t)", "text");
    item.replace("{N)", "AWSSecKey");
    item.replace("{id}", "AWSSecKey");
    item.replace("{V)", AWSSecKey);
    item.replace("{pf)", "size=30 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;
    //////////////////////////////////////////////////////////////////////////////////////
  
    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "GCP Devide ID");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "GCP1");
    if(PlatformData != 3){item.replace("{s_d)", "display:none");}
    item.replace("{i_t)", "text");
    item.replace("{N)", "GCPDevice");
    item.replace("{id}", "GCPDevice");
    item.replace("{V)", GCPDevice);
    item.replace("{pf)", "size=30 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;

    item = FPSTR(HTTP_FIELDSET_PARAM);
    item.replace("{L)", "GCP Registry ID");
    item.replace("{i1)", HTTP_INPUT_PARAM);
    item.replace("{f_id)", "GCP2");
    if(PlatformData != 3){item.replace("{s_d)", "display:none");}
    item.replace("{i_t)", "text");
    item.replace("{N)", "GCPRegistry");
    item.replace("{id}", "GCPRegistry");
    item.replace("{V)", GCPRegistry);
    item.replace("{pf)", "size=30 autofocus");
    item.replace("{F)", "");
    item.replace("{i2)", "");
    item.replace("{i3)", "");
    item.replace("{i4)", "");
    item.replace("{i5)", "");
    item.replace("{i6)", "");
    Page += item;
    
  
  	item = FPSTR(HTTP_FIELDSET_PARAM);
  	item.replace("{L)", "Actions");
  	item.replace("{i1)", "<div>");
  	item.replace("{i2)", "<br>");
  	item.replace("{i3)", "<button id=\"set_button\" class=\"button\">Set</button>");
  	item.replace("{i4)", "</div>");
  	item.replace("{i5)", "");
  	item.replace("{i6)", "");
  	Page += item;
	
    Page +=   F("<div class=\"modal\" id=\"myModal\" style=\"display: none;\"><div class=\"modal-dialog\">"
                "<div class=\"modal-content\"><!-- Modal Header --><div class=\"modal-header cont\"><h4 class=\"modal-title\">Changes have been set</h4></div>"
                "<div class=\"modal-footer cont\">"
                "<button type=\"button\" class=\"btn btn-danger\" data-dismiss=\"modal\" id=\"closeBtn\">Confirm</button></div></div></div></div>"
                "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>"
                "<script>"
                "var url_encode; var url_encode1; var url_encode2; var url_encode3; var url_encode4; var url_encode5; var ConKey; var AWSEnd; var AWSConKey; var AWSSecKey; var GCPDevice; var GCPRegistry;"
                "$('input:radio[name=\"PlatformState\"]').change(function(){if($(PlatformState1).is(':checked')){$('#Azure').show(); $('#AWS1').hide(); $('#AWS2').hide(); $('#AWS3').hide(); $('#GCP1').hide(); $('#GCP2').hide();}else if($(PlatformState2).is(':checked')){$('#AWS1').show(); $('#AWS2').show(); $('#AWS3').show(); $('#GCP1').hide(); $('#GCP2').hide(); $('#Azure').hide();}else if($(PlatformState3).is(':checked')){$('#GCP1').show(); $('#GCP2').show(); $('#AWS1').hide(); $('#AWS2').hide(); $('#AWS3').hide(); $('#Azure').hide();}});"           
                "$('#set_button').click(function(OnEvent){ OnEvent.preventDefault();"          
                "ConKey = $('#ConKey').val(); AWSEnd = $('#AWSEnd').val(); AWSConKey = $('#AWSConKey').val(); AWSSecKey = $('#AWSSecKey').val(); GCPDevice = $('#GCPDevice').val(); GCPRegistry = $('#GCPRegistry').val();"
                "url_encode = encodeURIComponent(ConKey); url_encode1 = encodeURIComponent(AWSEnd); url_encode2 = encodeURIComponent(AWSConKey); url_encode3 = encodeURIComponent(AWSSecKey); url_encode4 = encodeURIComponent(GCPDevice); url_encode5 = encodeURIComponent(GCPRegistry);"

                // Switch Platform
                "var PlatformState = $('input[name=PlatformState]:checked', '#ConKeyForm').val();"
                "$.get('/SetPlatform?PlatformState=' + PlatformState, function(PlatformState){ console.log(PlatformState); }); "  
                //
                "$.get('/SetConKey?url_encode=' + url_encode, function(url_encode){ console.log(url_encode);});"
                "$.get('/SetAwsKey?url_encode1=' + url_encode1, function(url_encode1){ console.log(url_encode1);});"
                "$.get('/SetAwsCon?url_encode2=' + url_encode2, function(url_encode2){ console.log(url_encode2);});"
                "$.get('/SetAwsSec?url_encode3=' + url_encode3, function(url_encode3){ console.log(url_encode3);});"
                "$.get('/SetGcpDev?url_encode4=' + url_encode4, function(url_encode4){ console.log(url_encode4);});"
                "$.get('/SetGcpReg?url_encode5=' + url_encode5, function(url_encode5){ console.log(url_encode5);});"
                
                "$('#myModal').show();});"
                "$('#closeBtn').click(function(OnEvent){ $('#myModal').hide();});"
                "</script>"
                "</body>"
                "</html>");

                
                server.send ( 200, "text/html", Page);
}




void handleSettings() {
  
      if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
      }
      bool DweetData;
      EEPROM_readAnything(163, DweetData);

        String Page = FPSTR(HTTP_HEADER);
      Page += FPSTR(HTTP_HEADER_PARAM);
      Page.replace("{h1}", "Setup to broadcast IP address");
      Page.replace("{h2}", "");
      Page.replace("{h3}", "");
      Page.replace("{h4}", "");
      Page.replace("{f_i)", "dweetNameForm");
    
        String item = FPSTR(HTTP_FIELDSET_PARAM);
      item.replace("{L)", "Dweet IP");
      item.replace("{i1)", HTTP_INPUT_PARAM);
      item.replace("{i_t)", "radio");
      item.replace("{N)", "IPState");
      item.replace("{id}", "IPState");
      item.replace("{V)", "1");
      if(DweetData){item.replace("{pf)", "checked");}
      else{item.replace("{pf)", "");}
      item.replace("{F)", " ON");
      item.replace("{i2)", HTTP_INPUT_PARAM); 
      item.replace("{i_t)", "radio");
      item.replace("{N)", "IPState");
      item.replace("{id}", "IPState");
      item.replace("{V)", "O");
      if(!DweetData){item.replace("{pf)", "checked");}
      else{item.replace("{pf)", "");}
      item.replace("{F)", " OFF");
      item.replace("{i3)", ""); 
      item.replace("{i4)", ""); 
      item.replace("{i5)", ""); 
      item.replace("{i6)", "");   
      Page += item; 
    
      item = FPSTR(HTTP_FIELDSET_PARAM);
      item.replace("{L)", "Actions");
      item.replace("{i1)", "<div>");
      item.replace("{i2)", "<br>");
      item.replace("{i3)", "<button id=\"set_button\" class=\"button\">Set</button>");
      item.replace("{i4)", "</div>");
      item.replace("{i5)", ""); 
      item.replace("{i6)", "");
      Page += item; 
  
      Page +=  F("</br><p><a id=\"myIPAnchor\" href='"); 
      Page += F("https://dweet.io/get/latest/dweet/for/");
      Page += WiFi.macAddress();
      
      Page += F("'>Link to get IP address</a></p>"
          "</br>"
          "<a href=FactorySettings>Restore Factory Settings</a>"
          "<div class=\"modal\" id=\"myModal\" style=\"display: none;\"><div class=\"modal-dialog\">"
          "<div class=\"modal-content\"><!-- Modal Header --><div class=\"modal-header cont\"><h4 class=\"modal-title\">Changes have been set</h4></div>"
          "<div class=\"modal-footer cont\">"
          "<button type=\"button\" class=\"btn btn-danger\" data-dismiss=\"modal\" id=\"closeBtn\">Confirm</button></div></div></div></div>"
          "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>"
          "<script>"
          "$('#set_button').click(function(OnEvent){ OnEvent.preventDefault();"
          "var IPState = $('input[name=IPState]:checked', '#dweetNameForm').val();"
          "$.get('/SetDweet?IPState=' + IPState, function(Dweet){ console.log(Dweet); }); "    
          "$('#myModal').show();});"
          "$('#closeBtn').click(function(OnEvent){ $('#myModal').hide();});"
          "document.getElementById(\"myIPAnchor\").href = b;"
          "</script>"
          "</body>"
          "</html>");
          server.send ( 200, "text/html", Page); 
}
