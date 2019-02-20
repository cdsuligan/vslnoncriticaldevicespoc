#include <Arduino.h>
#include <Stream.h>
#include <EEPROM.h>
//#include <TimerOne.h>
#include "EEPROMAnything.h"
#include "setpins.h"
#include "dweet_ip.h"
#include "web_portal.h"
#include "currentMonitor.h"
#include "restore_factory_settings.h"
#include <FS.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "WiFiManager2.h"
#include <AzureIoTHub.h>
#define MESSAGE_MAX_LEN 256




///////////*GLOBAL VARIABLES*//////////
char IOT_CONFIG_CONNECTION_STRING[190];
static struct Reporting PinReporting;
static bool DweetData;
static struct PinStatus LastPinStatus = {true}; //initialise to true to send out startup state


static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
static char factory_settings_stored [3];
static bool ConKey;
static unsigned int messageCount = 1;
static unsigned int ipcount = 540;
static int iotcount = -50;
static int platformMemory;
static unsigned int WiFiCount = 0;


#include "GcpConnect.h"
#include "AwsConnect.h"
#include "AzureConnect.h"


void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  EEPROM_readAnything(155, factory_settings_stored);

  if (memcmp(&factory_settings_stored, "YES", 3) != 0) {
    restore_factory_settings();
  }
  struct ESPPins PinSet;
  EEPROM_readAnything(128, PinSet);
  SetPins(PinSet);


  /////WiFi
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect("AutoConnectAP", "administrator");
  start_server();


  Serial.println (F("HTTP server started"));
  EEPROM_readAnything(140, platformMemory);
  initTime();

  //////Different Platforms
  if (platformMemory == 1) { // When the user has chosen Azure IoT Central
    Serial.println ("---------------------------------");
    Serial.println ("User has chosen Azure IoT Central");
    Serial.println ("---------------------------------");
    EEPROM_readAnything(163, IOT_CONFIG_CONNECTION_STRING);
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol);
    if (iotHubClientHandle == NULL) {
      Serial.println(F("Failed on IoTHubClient_CreateFromConnectionString"));
      ConKey = false;
    } else {
      ConKey = true;
    }

  } else if (platformMemory == 2) { //////////////////// When the user has chosen AWS
    Serial.println ("-------------------");
    Serial.println ("User has chosen AWS");
    Serial.println ("-------------------");
    EEPROM_readAnything(353, aws_endpoint);// will be approx 55 char 
    EEPROM_readAnything(408, aws_key); // will be approx 25 char
    EEPROM_readAnything(433, aws_secret); // will be approx 45 char
    //fill AWS parameters
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);
    ///
    if (connect()) {
        subscribe();
      } else {
      Serial.println("Cannot connect to MQTT");
      }

  } else if (platformMemory == 3) { ///////////////When the user has chosen GCP
    Serial.println ("-------------------");
    Serial.println ("User has chosen GCP");
    Serial.println ("-------------------");
    EEPROM_readAnything(478, device_id); // must be less then 15 char
    EEPROM_readAnything(495, registry_id); // must be less then 15 char     
    setupCloudIoT(); // Creates globals for MQTT
    pinMode(LED_BUILTIN, OUTPUT);
    startMQTT();
  }

  //read all settings from EEPROM
  EEPROM_readAnything(0, PinReporting);
  EEPROM_readAnything(163, DweetData);


  //DweetIP(); /////////////////re-introduce after prototyping finished //////////////

  pinMode(D0, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  //pinMode(A0,INPUT);//not sure if this is required

  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(5000000); //1s
}

unsigned long lastMillis = 0;

void loop() {
  handle_client();

  if (iotcount >= PinReporting.frequency ) {
    if (platformMemory == 1 ) { /////When user has chosen Azure IoT
      if (PinStatusChange() && ConKey ) { ///Only sends data when PinStatusChange
        Serial.println ("Pin status changed. Sending new status");
        char messagePayload[MESSAGE_MAX_LEN];
        readMessage(messageCount, messagePayload);
        sendMessage(iotHubClientHandle, messagePayload);
        IoTHubFullSendReceive();
        messageCount++;
      }
      ////////////Sends AC value
      Serial.println ("Sending AC value");
      char payloadAC[MESSAGE_MAX_LEN];
      readMessageAC(messageCount, payloadAC);
      sendMessage(iotHubClientHandle, payloadAC);
      IoTHubFullSendReceive();
      messageCount++;
      /////////////////////
    } else if (platformMemory == 2 ) { /////When user has chosen AWS
      Serial.println ("Sending data to AWS");
      if (connect()) {
        sendAWSMessage();
      } else {
      Serial.println("Cannot connect to MQTT");
      }

    } else if (platformMemory == 3 ) {
      mqttClient->loop();
      delay(10);  // <- fixes some issues with WiFi stability
    
      if (!mqttClient->connected()) {
        mqttConnect();
      }
      // publish a message roughly every second.
      if (millis() - lastMillis > 1000) {
        lastMillis = millis();
        Serial.println("Every Seconds");
        publishTelemetry();
      }
    }

    iotcount = 0;
  }

  if (ipcount >= 600)
  {
    if (DweetData)
    {
      char PubIP [100];
      if (getIp(PubIP))
      {
        char mac [15];
        char locIP [10];
        uint8_t temp [6];
        WiFi.macAddress(temp);
        sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
        sprintf(locIP, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
        dweet(PubIP, mac, locIP);
      }
    }
    ipcount = 0;
  }

  if (WiFiCount >= 5)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      //////////////////////////try to reconnect rather than resetting////////////////////
      ESP.restart();
    }
    WiFiCount = 0;
  }
}
