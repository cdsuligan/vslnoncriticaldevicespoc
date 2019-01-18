#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "setpins.h"
#include "dweet_ip.h"
#include "web_portal.h"
#include "currentMonitor.h"
#include "restore_factory_settings.h"
#include <FS.h>   
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include "WiFiManager2.h"

	struct PinStatus {
	  bool D0Status;
	  bool D1Status;
	  bool D2Status;
	  bool A0Status;
	};

	#define MESSAGE_MAX_LEN 128
	
	///////////*GLOBAL VARIABLES*//////////
	char IOT_CONFIG_CONNECTION_STRING[200];
	static struct Reporting PinReporting;
	static bool DweetData;
	static struct PinStatus LastPinStatus = {true}; //initialise to true to send out startup state
	static char factory_settings_stored [3];
	static bool ConKey;
	static unsigned int messageCount = 1;
	static unsigned int ipcount = 540;
	static int iotcount = -50;
	static unsigned int WiFiCount = 0;


	static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;


	void ICACHE_RAM_ATTR onTimerISR(){
		  WiFiCount++;
		  iotcount++;
		  ipcount++;
		  timer1_write(5000000);//1s
	}

void setup() {
    Serial.begin(115200);
    EEPROM.begin(512);
    EEPROM_readAnything(200, factory_settings_stored);
    if(memcmp(&factory_settings_stored,"YES",3) != 0)
    {
      restore_factory_settings();
    }
    struct ESPPins PinSet;
    EEPROM_readAnything(128, PinSet);
    SetPins(PinSet);
    //WiFiManager
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    wifiManager.setConfigPortalTimeout(180); 
    // wifiManager.setSTAStaticIPConfig(IPAddress(192,168,1,240), IPAddress(192,168,1,1), IPAddress(0,0,0,0));
    wifiManager.autoConnect("AutoConnectAP", "administrator");
    start_server();
    Serial.println (F("HTTP server started"));
    initTime();
    EEPROM_readAnything(256,IOT_CONFIG_CONNECTION_STRING);
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println(F("Failed on IoTHubClient_CreateFromConnectionString"));
        ConKey = false; 
    }
    else{ConKey = true;}
    
     //read all settings from EEPROM
    EEPROM_readAnything(0,PinReporting);
    EEPROM_readAnything(150, DweetData);

    //DweetIP(); /////////////////re-introduce after prototyping finished //////////////

    pinMode(D0,INPUT_PULLUP);
    pinMode(D1,INPUT_PULLUP);
    pinMode(D2,INPUT_PULLUP);
    //pinMode(A0,INPUT);//not sure if this is required 
    


   timer1_attachInterrupt(onTimerISR);
   timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
   timer1_write(5000000); //1s
}


void loop() {
  
     handle_client();
     
	    if (iotcount >= PinReporting.frequency)
        {
          if(PinStatusChange() && ConKey)
            { 
              char messagePayload[MESSAGE_MAX_LEN];
              readMessage(messageCount, messagePayload);
              sendMessage(iotHubClientHandle, messagePayload);
              IoTHubFullSendReceive();
              messageCount++;
            } 
			   iotcount = 0;
        }

      if(ipcount >= 600)
      {
         if(DweetData)
            {     
              char PubIP [100];
              if (getIp(PubIP))
                  {
                     char mac [20];
                     char locIP [10];
                     uint8_t temp [6];
                     WiFi.macAddress(temp);
                     sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
                     sprintf(locIP,"%d.%d.%d.%d", WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
                     dweet(PubIP,mac,locIP);
                  }
            }
        ipcount = 0;
      }     
      
     if(WiFiCount >= 5)
     {
        if(WiFi.status() != WL_CONNECTED)
           {
            //////////////////////////try to reconnect rather than resetting////////////////////
              ESP.restart();
           }
        WiFiCount = 0;
     }
}

void IoTHubFullSendReceive() 
{
  IOTHUB_CLIENT_STATUS Status;
  static unsigned int watchdog;
  while ((IoTHubClient_LL_GetSendStatus(iotHubClientHandle, &Status) == IOTHUB_CLIENT_OK) && (Status == IOTHUB_CLIENT_SEND_STATUS_BUSY))
  {
      IoTHubClient_LL_DoWork(iotHubClientHandle);
      ThreadAPI_Sleep(100);
      watchdog++;
      yield(); ////////////////////////////////////////////////need to deal with this better. possibly leave routine and return later///////////////////////////////////////
        if(watchdog > 100)
          {
          watchdog = 0;
          Serial.println (F("Communication with iotHub Failed!"));
          ESP.restart();
          }
  }
}


void initTime() {

    static unsigned int watchdog;
    
    time_t epochTime;

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true) {
        epochTime = time(NULL);

        if (epochTime == 0) {
            Serial.println(F("Fetching NTP epoch time failed! Waiting to retry"));
            delay(1000);
            watchdog++;
        } else {
            Serial.print(F("Fetched NTP epoch time is: "));
            Serial.println(epochTime);
            break;
        }
        if(watchdog >= 20)
        {
            //ESP.restart();
            break;
        }
    }
}

void readMessage(unsigned int messageId, char *payload)
{
   StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
   JsonObject &object = jsonBuffer.createObject();
   object[PinReporting.messageID] = messageId;
   if(PinReporting.D0State){
     object[PinReporting.D0Name] = String(LastPinStatus.D0Status);
   }
   if(PinReporting.D1State){
     object[PinReporting.D1Name] = String(LastPinStatus.D1Status);
   }
   if(PinReporting.D2State){
     object[PinReporting.D2Name] = String(LastPinStatus.D2Status);
   }
   if(PinReporting.A0State){
     object[PinReporting.A0Name] = String(LastPinStatus.A0Status);
   }

   Serial.print("AC: ");
   double A0Value = calcIrms() * PinReporting.A0Scale;
   Serial.println(A0Value);
   object["AC"]=A0Value;

   object.printTo(payload, MESSAGE_MAX_LEN); //inserting data
}



static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, char *buffer)
{
    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray((const unsigned char *)buffer, strlen(buffer));
    if (messageHandle == NULL)
    {
        Serial.println(F("Unable to create a new IoTHubMessage."));
    }
    else
    {
        Serial.printf("Sending message: %s.\r\n", buffer);
        if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, NULL) != IOTHUB_CLIENT_OK)
        {
            Serial.println(F("Failed to hand message to IoTHubClient"));
        }
        else
        {
            Serial.println(F("IoTHubClient available for delivery"));
        }
        IoTHubMessage_Destroy(messageHandle);
    }
}

static void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
    if (IOTHUB_CLIENT_CONFIRMATION_OK == result)
    {
        Serial.println(F("Message sent to IoT Hub"));
    }
    else
    {
        Serial.println(F("Failed message to IoT Hub"));
    }
    
}

char PinStatusChange() {
 
     struct PinStatus ThisPinStatus;
     memset(&ThisPinStatus, 0, sizeof ThisPinStatus);
      
      if(PinReporting.D0State){
          ThisPinStatus.D0Status = digitalRead(D0);
      }
      else {
        ThisPinStatus.D0Status = false;
      }
      if(PinReporting.D1State){
          ThisPinStatus.D1Status = digitalRead(D1);
      }
      else {
        ThisPinStatus.D1Status = false;
      }
      if(PinReporting.D2State){
          ThisPinStatus.D2Status = digitalRead(D2);
      }  
      else {
        ThisPinStatus.D2Status = false;
      }
      if(PinReporting.A0State == 2){
          Serial.print("AC: ");
          double A0Value = calcIrms() * PinReporting.A0Scale;
          Serial.println(A0Value);
          if(A0Value > PinReporting.A0Threshold)
          {
            ThisPinStatus.A0Status = true;
          }
          else
          {
            ThisPinStatus.A0Status = false;
          }
      } 
      else if(PinReporting.A0State == 1){
          Serial.print("DC: ");
          double A0Value = analogRead(A0) * PinReporting.A0Scale;
          Serial.println(A0Value);
          if(A0Value > PinReporting.A0Threshold)
          {
            ThisPinStatus.A0Status = true;
          }
          else
          {
            ThisPinStatus.A0Status = false;
          }
      }
      else {
        ThisPinStatus.A0Status = false;
      }
    
      if(memcmp(&ThisPinStatus,&LastPinStatus,sizeof(ThisPinStatus)) == 0)
      {
        return(0);
      }
      else
      {
        memset(&LastPinStatus, 0, sizeof LastPinStatus);
        LastPinStatus = ThisPinStatus;
        return(1);
      }
}
