#include <Arduino.h>
#include <Stream.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <AmazonIOTClient.h>

//AWS
#include "sha256.h"
#include "Utils.h"

//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>

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

//static Reporting PinReporting;
//static PinStatus LastPinStatus = {true};

//////AWS credentials
char aws_endpoint[55];
char aws_key[25];
char aws_secret[45];
const char* aws_region = "ap-southeast-2";
const char* aws_topic = "$aws/things/IoTTestArduino/shadow/update";
int port = 443;



///////MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

///////AWS config
AWSWebSocketClient awsWSclient(1000);
PubSubClient client(awsWSclient);
long connection = 0;

//generate random mqtt clientID
char* generateClientID () {
  char* cID = new char[23]();
  for (int i = 0; i < 22; i += 1)
    cID[i] = (char)random(1, 256);
  return cID;
}

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

  //char* cTemp = "90";
  //char* cHumid = "55";
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
    String(LastPinStatus.D0Status).toCharArray(dZero, 256);
  }
  if (PinReporting.D1State) {
    String(LastPinStatus.D1Status).toCharArray(dOne, 256);
  }
  if (PinReporting.D2State) {
    String(LastPinStatus.D2Status).toCharArray(dTwo, 256) ;
  }
  if (PinReporting.A0State) {
    String(LastPinStatus.A0Status).toCharArray(aZero, 256) ;
  }

  strcpy(shadow, "{\"state\":{\"reported\": {\"D0Status\":");
  strcat(shadow, dZero);
  strcat(shadow, ", \"D1Status\":");
  strcat(shadow, dOne);
  strcat(shadow, ", \"D2Status\":");
  strcat(shadow, dTwo);
  strcat(shadow, ", \"A0Status\":");
  strcat(shadow, aZero);
  strcat(shadow, ", \"AC\":");
  strcat(shadow, aa);
  strcat(shadow, "}}}");

  int rc = client.publish(aws_topic, shadow);
  Serial.println(shadow);
  delay (6000);
}
