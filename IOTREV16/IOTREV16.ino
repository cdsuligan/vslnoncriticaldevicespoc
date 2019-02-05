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
#include <Arduino.h>
#include <Stream.h>
#include <ESP8266WiFiMulti.h>
#include "AmazonIOTClient.h"

//AWS
#include "sha256.h"
#include "Utils.h"

//WEBSockets
#include <Hash.h>
#include "WebSocketsClient.h"

//MQTT PUBSUBCLIENT LIB
#include <PubSubClient.h>

//AWS MQTT Websocket
#include "Client.h"
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"

AmazonIOTClient iotClient;
ActionError actionError;

extern "C" {
#include "user_interface.h"
}

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
static int platformMemory;
static unsigned int WiFiCount = 0;
static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

//////AWS credentials
char aws_endpoint[]    = "a3pidz6hu6hupb-ats.iot.ap-southeast-2.amazonaws.com";
char aws_key[]         = "AKIAJS5X2IKLMZWMPZSQ";
char aws_secret[]      = "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";
char aws_region[]      = "ap-southeast-2";
const char* aws_topic  = "$aws/things/IoTTestArduino/shadow/update";
int port = 443;

///////MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

///////AWS config
AWSWebSocketClient awsWSclient(1000);
PubSubClient client(awsWSclient);

//# of connections
long connection = 0;

void ICACHE_RAM_ATTR onTimerISR() {
  WiFiCount++;
  iotcount++;
  ipcount++;
  timer1_write(5000000);//1s
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  EEPROM_readAnything(200, factory_settings_stored);

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
  EEPROM_readAnything(205, platformMemory);
  Serial.println (platformMemory);
  initTime();

  //////Different Platforms
  if (platformMemory == 1) { // When the user has chosen Azure IoT Central
    Serial.println ("User has chosen Azure IoT Central");
    EEPROM_readAnything(256, IOT_CONFIG_CONNECTION_STRING);
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
      Serial.println(F("Failed on IoTHubClient_CreateFromConnectionString"));
      ConKey = false;
    }
    else {
      ConKey = true;
    }
    //////////////////// When the user has chosen AWS
  } else if (platformMemory == 2) {
    Serial.println ("User has chosen AWS");
    //fill AWS parameters
    awsWSclient.setAWSRegion(aws_region);
    awsWSclient.setAWSDomain(aws_endpoint);
    awsWSclient.setAWSKeyID(aws_key);
    awsWSclient.setAWSSecretKey(aws_secret);
    awsWSclient.setUseSSL(true);

    ///////////////When the user has chosen GCP
  } else if (platformMemory == 3) {
    Serial.println ("User has chosen GCP");
    EEPROM_readAnything(256, IOT_CONFIG_CONNECTION_STRING);
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONFIG_CONNECTION_STRING, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
      Serial.println(F("Failed on IoTHubClient_CreateFromConnectionString"));
      ConKey = false;
    }
    else {
      ConKey = true;
    }
  }

  //read all settings from EEPROM
  EEPROM_readAnything(0, PinReporting);
  EEPROM_readAnything(150, DweetData);

  //DweetIP(); /////////////////re-introduce after prototyping finished //////////////

  pinMode(D0, INPUT_PULLUP);
  pinMode(D1, INPUT_PULLUP);
  pinMode(D2, INPUT_PULLUP);
  //pinMode(A0,INPUT);//not sure if this is required

  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(5000000); //1s
}


void loop() {

  handle_client();

  if (iotcount >= PinReporting.frequency ) {
    if (platformMemory == 1 ) {
      if (PinStatusChange() && ConKey ) { ///Only sends data when PinStatusChange
        Serial.println ("11111111111111111111111111111");
        char messagePayload[MESSAGE_MAX_LEN];
        readMessage(messageCount, messagePayload);
        sendMessage(iotHubClientHandle, messagePayload);
        IoTHubFullSendReceive();
        messageCount++;
      }

      ////////////Sends AC value
      Serial.println ("This will send AC value");
      char payloadAC[MESSAGE_MAX_LEN];
      readMessageAC(messageCount, payloadAC);
      sendMessage(iotHubClientHandle, payloadAC);
      IoTHubFullSendReceive();
      messageCount++;
      /////////////////////
    } else if (platformMemory == 2 ) {
      Serial.println ("2222222222222222222222222222222222222222222222222222222222");
      if (connect()) {
        subscribe();
        sendAWSMessage();
      }

    } else if (platformMemory == 3 ) {
      if (PinStatusChange() && ConKey ) {
        Serial.println ("3333333333333333333333333333");
        char messagePayload[MESSAGE_MAX_LEN];
        readMessage(messageCount, messagePayload);
        sendMessage(iotHubClientHandle, messagePayload);
        IoTHubFullSendReceive();
        messageCount++;
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
        char mac [20];
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
    if (watchdog > 100)
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
    if (watchdog >= 20)
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
  if (PinReporting.D0State) {
    object[PinReporting.D0Name] = String(LastPinStatus.D0Status);
  }
  if (PinReporting.D1State) {
    object[PinReporting.D1Name] = String(LastPinStatus.D1Status);
  }
  if (PinReporting.D2State) {
    object[PinReporting.D2Name] = String(LastPinStatus.D2Status);
  }
  if (PinReporting.A0State) {
    object[PinReporting.A0Name] = String(LastPinStatus.A0Status);
  }

  object.printTo(payload, MESSAGE_MAX_LEN); //inserting data
}

void readMessageAC(unsigned int messageId, char *payload)
{
  StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
  JsonObject &object = jsonBuffer.createObject();
  object[PinReporting.messageID] = messageId;

  Serial.print("AC: ");
  double A0Value = calcIrms() * PinReporting.A0Scale;
  Serial.println(A0Value);
  object["AC"] = A0Value;

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

//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i = 0; i < 22; i += 1)
    cID[i] = (char)random(1, 256);
  return cID;
}

//callback to handle mqtt messages
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//connects to websocket layer and mqtt layer
bool connect () {
  if (client.connected()) {
    client.disconnect ();
  }
  //delay is not necessary... it just help us to get a "trustful" heap space value
  delay (1000);
  Serial.print (millis ());
  Serial.print (" - conn: ");
  Serial.print (++connection);
  Serial.print (" - (");
  Serial.print (ESP.getFreeHeap ());
  Serial.println (")");

  //creating random client id
  char* clientID = generateClientID ();

  client.setServer(aws_endpoint, port);
  if (client.connect(clientID)) {
    Serial.println("connected");
    return true;
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    return false;
  }
}

//subscribe to a mqtt topic
void subscribe () {
  client.setCallback(callback);
  client.subscribe(aws_topic);
  //subscript to a topic
  Serial.println("MQTT subscribed");
}

//send a message to a mqtt topic
void sendAWSMessage() {

  char* cTemp = "90";
  char* cHumid = "55";
  char shadow[256];
  double A0Value = calcIrms() * PinReporting.A0Scale;
  String aZeroValue = String(A0Value);
  char aa[256];
  aZeroValue.toCharArray(aa, 256);
  char dZero[256];
  char dOne[256];
  char dTwo[256];
  char aZero[256];

  if (PinReporting.D0State) {
    String(LastPinStatus.D0Status).toCharArray(dZero,256);
  }
  if (PinReporting.D1State) {
    String(LastPinStatus.D1Status).toCharArray(dOne,256);
  }
  if (PinReporting.D2State){
    String(LastPinStatus.D2Status).toCharArray(dTwo,256) ;
  }
  if (PinReporting.A0State){
    String(LastPinStatus.A0Status).toCharArray(aZero,256) ;
  }

  strcpy(shadow, "{\"state\":{\"reported\": {\"Humidity\":");
  strcat(shadow, dZero);
  strcat(shadow, ", \"AC\":");
  strcat(shadow, aa);
  strcat(shadow, ", \"Temperature\":");
  strcat(shadow, cTemp);
  strcat(shadow, "}}}");

  int rc = client.publish(aws_topic, shadow);
  Serial.println(shadow);
  delay (6000);
}

char PinStatusChange() {

  struct PinStatus ThisPinStatus;
  memset(&ThisPinStatus, 0, sizeof ThisPinStatus);

  if (PinReporting.D0State) {
    ThisPinStatus.D0Status = digitalRead(D0);
  }
  else {
    ThisPinStatus.D0Status = false;
  }
  if (PinReporting.D1State) {
    ThisPinStatus.D1Status = digitalRead(D1);
  }
  else {
    ThisPinStatus.D1Status = false;
  }
  if (PinReporting.D2State) {
    ThisPinStatus.D2Status = digitalRead(D2);
  }
  else {
    ThisPinStatus.D2Status = false;
  }
  if (PinReporting.A0State == 2) {
    Serial.print("AC: ");
    double A0Value = calcIrms() * PinReporting.A0Scale;
    Serial.println(A0Value);
    if (A0Value > PinReporting.A0Threshold)
    {
      ThisPinStatus.A0Status = true;
    }
    else
    {
      ThisPinStatus.A0Status = false;
    }
  }
  else if (PinReporting.A0State == 1) {
    Serial.print("DC: ");
    double A0Value = analogRead(A0) * PinReporting.A0Scale;
    Serial.println(A0Value);
    if (A0Value > PinReporting.A0Threshold)
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

  if (memcmp(&ThisPinStatus, &LastPinStatus, sizeof(ThisPinStatus)) == 0)
  {
    return (0);
  }
  else
  {
    memset(&LastPinStatus, 0, sizeof LastPinStatus);
    LastPinStatus = ThisPinStatus;
    return (1);
  }
}
