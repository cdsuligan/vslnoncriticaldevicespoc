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

const char HTTP_FIELDSET_PARAM[] PROGMEM  = "<fieldset><legend>{L)</legend>{i1){i2){i3){i4){i5){i6)</fieldset>";
										
const char HTTP_INPUT_PARAM[] PROGMEM  =	"<input type='{i_t)'name='{N)'id='{id}'value='{V)'{pf)>{F)";								

const char HTTP_HEADER_PARAM[] PROGMEM  = "<body><h1>{h1}</h1><h2>{h2}</h2><h3>{h3}</h3><h4>{h4}</h4><form id='{f_i)'>";	

/*
   server.setContentLength(sizeof(HTTP_HEADER) + sizeof(SETTINGS_SAVED)); //make static length if known to be constant
   server.send ( 200, "text/html", HTTP_HEADER ); 
   server.sendContent(SETTINGS_SAVED);
*/ 



void start_server(){
    server.on ("/", handleRoot);
    server.on ("/Reporting", handleReporting);
    server.on ("/setReporting", handleSaveReport);
    server.on ("/set", handleSet);
    server.on ("/PinSet", handlePinSet);
    server.on ("/ResetESP", ResetESP);
    server.on ("/FactorySettings", FactorySettings); 
    server.on ("/Settings", handleSettings);
    server.on ("/ConKey", handleConKey);
    server.on ("/SetConKey", handleSaveConKey);    
    server.on ("/SetDweet", handleSaveDweet);
    server.on ("/SetPlatform", savePlatformState);
    
    server.begin();
}

void handle_client(){
server.handleClient();
}


void handleRoot(){
    if (!server.authenticate(www_username, www_password)) {
      return server.requestAuthentication();
    }

String Page = FPSTR(HTTP_HEADER);
Page +=	F("<body>"
  			"<h1 align=\"center\">Eff% IoT</h1>"
			"<h3>Please select a button below</h3>"
  			"<a href=PinSet>Pin Set</a>"
  			"</br>"
  			"<a href=Reporting>Input Analytics</a>"
  			"</br>"
  			"<a href=ConKey>IOT Connection Key</a>"
  			"</br>"
  			"<a href=Settings>Settings</a>"
  			"</body>"
  			"</html>"); 
   server.sendHeader("Content-Length", String(Page.length())); 
   server.send ( 200, "text/html", Page );  
}

void handleReporting() {

  
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
    struct Reporting PinReporting;
    EEPROM_readAnything(0,PinReporting);
     
	String Page = FPSTR(HTTP_HEADER);
	Page += FPSTR(HTTP_HEADER_PARAM);
	Page.replace("{h1}", "Azure Setup");
	Page.replace("{h2}", "IOT Central Input Analytics");
	Page.replace("{h3}", "");
	Page.replace("{h4}", "");
	Page.replace("{f_i)", "Pinform");
	
	String item = FPSTR(HTTP_FIELDSET_PARAM);
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
	
Page +=	F("<div class=\"modal\" id=\"myModal\" style=\"display: none;\"><div class=\"modal-dialog\">"
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
        "</script></body></html>");

   server.send ( 200, "text/html", Page); 
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

void handleSet() {

  struct ESPPins WebPortalPinSet;

  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }

  if(server.arg("D3State") == "D")
  {
    WebPortalPinSet.D3 = true;
  }
  else if(server.arg("D3State") == "O")
  {
    WebPortalPinSet.D3 = false;
  }

  if(server.arg("D4State") == "D")
  {
    WebPortalPinSet.D4 = true;
  }
  else if(server.arg("D4State") == "O")
  {
    WebPortalPinSet.D4 = false;
  }

  if(server.arg("D5State") == "D")
  {
    WebPortalPinSet.D5 = true;
  }
  else if(server.arg("D5State") == "O")
  {
    WebPortalPinSet.D5 = false;
  }

  if(server.arg("D6State") == "A")
  {
    WebPortalPinSet.A6 = server.arg("D6Value").toInt();
  }
  else if(server.arg("D6State") == "O")
  {
    WebPortalPinSet.A6 = 0;
  }
  
   EEPROM_writeAnything(128, WebPortalPinSet);
   SetPins(WebPortalPinSet);
}

void handlePinSet() {

  
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
	struct ESPPins ReadPinSet;
	EEPROM_readAnything(128,ReadPinSet);
  
  
  	String Page = FPSTR(HTTP_HEADER);
	Page += FPSTR(HTTP_HEADER_PARAM);
	Page.replace("{h1}", "GPIO Pin Set");
	Page.replace("{h2}", "toggle an output on or off");
	Page.replace("{h3}", "");
	Page.replace("{h4}", "");
	Page.replace("{f_i)", "Pinform");
	
	String item = FPSTR(HTTP_FIELDSET_PARAM);
	item.replace("{L)", "Digital Output 3");
	item.replace("{i1)", HTTP_INPUT_PARAM);
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D3State");
	item.replace("{id}", "D3State");
	item.replace("{V)", "D");
	if(ReadPinSet.D3){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " ON");
	item.replace("{i2)", HTTP_INPUT_PARAM); 
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D3State");
	item.replace("{id}", "D3State");
	item.replace("{V)", "O");
	if(!ReadPinSet.D3){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " OFF");
	item.replace("{i3)", ""); 	
	item.replace("{i4)", "");	
	item.replace("{i5)", "");	
	item.replace("{i6)", "");		
	Page += item;

	item = FPSTR(HTTP_FIELDSET_PARAM);
	item.replace("{L)", "Digital Output 4");
	item.replace("{i1)", HTTP_INPUT_PARAM);
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D4State");
	item.replace("{id}", "D4State");
	item.replace("{V)", "D");
	if(ReadPinSet.D4){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " ON");
	item.replace("{i2)", HTTP_INPUT_PARAM); 
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D4State");
	item.replace("{id}", "D4State");
	item.replace("{V)", "O");
	if(!ReadPinSet.D4){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " OFF");
	item.replace("{i3)", ""); 
	item.replace("{i4)", "");	
	item.replace("{i5)", "");	
	item.replace("{i6)", "");		
	Page += item;	
    
 	item = FPSTR(HTTP_FIELDSET_PARAM);
	item.replace("{L)", "Digital Output 5");
	item.replace("{i1)", HTTP_INPUT_PARAM);
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D5State");
	item.replace("{id}", "D5State");
	item.replace("{V)", "D");
	if(ReadPinSet.D5){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " ON");
	item.replace("{i2)", HTTP_INPUT_PARAM); 
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D5State");
	item.replace("{id}", "D5State");
	item.replace("{V)", "O");
	if(!ReadPinSet.D5){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " OFF");
	item.replace("{i3)", ""); 	
	item.replace("{i4)", "");	
	item.replace("{i5)", "");	
	item.replace("{i6)", "");		
	Page += item; 
  
	item = FPSTR(HTTP_FIELDSET_PARAM);
	item.replace("{L)", "Analogue Output D6");
	item.replace("{i1)", HTTP_INPUT_PARAM);
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D6State");
	item.replace("{id}", "D6State");
	item.replace("{V)", "D");
	if(ReadPinSet.A6 > 0){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " ON");
	item.replace("{i2)", HTTP_INPUT_PARAM); 
	item.replace("{i_t)", "radio");
	item.replace("{N)", "D6State");
	item.replace("{id}", "D6State");
	item.replace("{V)", "O");
	if(ReadPinSet.A6 == false){item.replace("{pf)", "checked");}
	else{item.replace("{pf)", "");}
	item.replace("{F)", " OFF &nbsp &nbsp");
	item.replace("{i3)", HTTP_INPUT_PARAM); 
	item.replace("{i_t)", "text");
	item.replace("{N)", "D6Value");
	item.replace("{id}", "D6Value");
	item.replace("{V)", String(ReadPinSet.A6)); 	
	item.replace("{pf)", "size=2 autofocus");
	item.replace("{F)", " VALUE");	
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
        "var D3State; var D4State; var D5State; var D6State; var D6Value; $('#set_button').click(function(OnEvent){ OnEvent.preventDefault();"
        "D3State = $('input[name=D3State]:checked', '#Pinform').val(); D4State = $('input[name=D4State]:checked', '#Pinform').val(); D5State = $('input[name=D5State]:checked', '#Pinform').val(); D6State = $('input[name=D6State]:checked', '#Pinform').val();  D6Value = $('#D6Value').val();"
        "$.get('/set?D3State=' + D3State + '&D4State=' + D4State + '&D5State=' + D5State + '&D6State=' + D6State + '&D6Value=' + D6Value, function(PinState){ console.log(PinState); });"
        "$('#myModal').show();});"
        "$('#closeBtn').click(function(OnEvent){ $('#myModal').hide();});"
        "</script>"
        "</body>"
        "</html>");

   server.send ( 200, "text/html", Page);  
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
    EEPROM_writeAnything(200, "NO");
    ESP.restart();
}

void handleSettings() {
  
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  
  bool DweetData;
  EEPROM_readAnything(150, DweetData);


  

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

void handleConKey() {
  
  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  
  char ConKey[200];
  EEPROM_readAnything(256, ConKey);

  int PlatformData;
  EEPROM_readAnything(205,PlatformData);

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
  item.replace("{id}", "PlatformState");
  item.replace("{V)", "1");
  if(PlatformData == 1){item.replace("{pf)", "checked");}
  else{item.replace("{pf)", "");}
  item.replace("{F)", "IoT Central  ");
  item.replace("{i2)", HTTP_INPUT_PARAM);
  item.replace("{i_t)", "radio");
  item.replace("{N)", "PlatformState");
  item.replace("{id}", "PlatformState");
  item.replace("{V)", "2");
  if(PlatformData == 2){item.replace("{pf)", "checked");}
  else{item.replace("{pf)", "");}
  item.replace("{F)", "AWS  ");
  item.replace("{i3)", HTTP_INPUT_PARAM);
  item.replace("{i_t)", "radio");
  item.replace("{N)", "PlatformState");
  item.replace("{id}", "PlatformState");
  item.replace("{V)", "3");
  if(PlatformData == 3){item.replace("{pf)", "checked");}
  else{item.replace("{pf)", "");}
  item.replace("{F)", "GCP");
  item.replace("{i4)", "");
  item.replace("{i5)", "");
  item.replace("{i6)", "");
  Page += item;
	
	String item1 = FPSTR(HTTP_FIELDSET_PARAM);
	item1.replace("{L)", "Azure IoT Central");
	item1.replace("{i1)", HTTP_INPUT_PARAM);
	item1.replace("{i_t)", "text");
	item1.replace("{N)", "ConKey");
	item1.replace("{id}", "ConKey");
	item1.replace("{V)", ConKey);
	item1.replace("{pf)", "size=30 autofocus");
	item1.replace("{F)", "");
  item1.replace("{i2)", "");
  item1.replace("{i3)", "");
  item1.replace("{i4)", "");
  item1.replace("{i5)", "");
  item1.replace("{i6)", "");
	Page += item1;

  String item2 = FPSTR(HTTP_FIELDSET_PARAM);
  item2.replace("{L)", "AWS");
  item2.replace("{i1)", HTTP_INPUT_PARAM);
  item2.replace("{i_t)", "text");
  item2.replace("{N)", "ConKey");
  item2.replace("{id}", "ConKey");
  item2.replace("{V)", ConKey);
  item2.replace("{pf)", "size=30 autofocus");
  item2.replace("{F)", "");
  item2.replace("{i2)", "");
  item2.replace("{i3)", "");
  item2.replace("{i4)", "");
  item2.replace("{i5)", "");
  item2.replace("{i6)", "");
  Page += item2;

  String item3 = FPSTR(HTTP_FIELDSET_PARAM);
  item3.replace("{L)", "GCP");
  item3.replace("{i1)", HTTP_INPUT_PARAM);
  item3.replace("{i_t)", "text");
  item3.replace("{N)", "ConKey");
  item3.replace("{id}", "ConKey");
  item3.replace("{V)", ConKey);
  item3.replace("{pf)", "size=30 autofocus");
  item3.replace("{F)", "");
  item3.replace("{i2)", "");
  item3.replace("{i3)", "");
  item3.replace("{i4)", "");
  item3.replace("{i5)", "");
  item3.replace("{i6)", "");
  Page += item3;


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
             "var url_encode; var ConKey; $('#set_button').click(function(OnEvent){ OnEvent.preventDefault();"
             "ConKey = $('#ConKey').val();"
             "url_encode = encodeURIComponent(ConKey);"
             "$.get('/SetConKey?url_encode=' + url_encode, function(url_encode){ console.log(url_encode);});"
             // Switch Platform
           "var PlatformState = $('input[name=PlatformState]:checked', '#ConKeyForm').val();"
             "$.get('/SetPlatform?PlatformState=' + PlatformState, function(Dweet){ console.log(Dweet); }); "
              //
             "$('#myModal').show();});"
             "$('#closeBtn').click(function(OnEvent){ $('#myModal').hide();});"
           "</script>"
         "</body>"
         "</html>");

   server.send ( 200, "text/html", Page);  
}

void handleSaveConKey() {

  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
   String s = server.arg("url_encode");
   char ConKey[200];
   strcpy(ConKey, s.c_str());
   ConKey[s.length()] = '\0'; //add the null terminator at the end of the string
   EEPROM_writeAnything(256, ConKey);   
}

void handleSaveDweet() {

  if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
   bool DweetData;
   if(server.arg("IPState") == "1"){
       DweetData = true;
    }
   else {DweetData = false;}
   EEPROM_writeAnything(150, DweetData);
}

void savePlatformState(){
    if (!server.authenticate(www_username, www_password)) {
    return server.requestAuthentication();
  }
  int platformNo;
  if(server.arg("PlatformState")=="1"){
    platformNo = 1; 
  } else if(server.arg("PlatformState")=="2"){
    platformNo = 2; 
  } else if(server.arg("PlatformState")=="3"){
    platformNo = 3; 
  }
  EEPROM_writeAnything(205, platformNo);
}
