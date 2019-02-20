/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
// This file contains static methods for API requests using Wifi / MQTT
#ifndef __ESP8266_MQTT_H__
#define __ESP8266_MQTT_H__
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "FS.h"
#include <time.h>
#include <rBase64.h>
#include <CloudIoTCore.h>
#include <MQTT.h>


// Holds Cloud IoT Core configuration
CloudIoTCoreDevice *device;

unsigned long iss = 0;
String jwt;
boolean wasErr;
WiFiClientSecure *netClient;
MQTTClient *mqttClient;

//////GCP credentials
char registry_id[15];
char device_id[15];
const char* project_id = "turnkey-life-229320";
const char* location = "asia-east1";
const char* private_key_str =
    "4f:fe:cd:5f:94:85:06:7b:43:52:1e:a3:ad:a0:e4:"
    "b0:f4:78:f1:64:33:e9:de:50:f5:43:b5:54:1a:6c:"
    "9a:06";



///////////////////////////////
// Helpers specific to this board
///////////////////////////////
String getDefaultSensor() {
  return  "Wifi: " + String(WiFi.RSSI()) + "db";
}

String getJwt() {
  if (iss == 0 || time(nullptr) - iss > 3600) {  // TODO: exp in device
    // Disable software watchdog as these operations can take a while.
    ESP.wdtDisable();
    iss = time(nullptr);
    Serial.println("Refreshing JWT");
    jwt = device->createJWT(iss);
    ESP.wdtEnable(0);
    Serial.println("Finished creating JWT");
  }
  return jwt;
}

void setupCert() {
  // Set CA cert on wifi client
  // If using a static (binary) cert:
  // netClient->setCACert_P(ca_crt, ca_crt_len);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Set CA cert from SPIFFS
  File ca = SPIFFS.open("/ca.crt", "r"); //replace ca.crt eith your uploaded file name
  if (!ca) {
    Serial.println("Failed to open ca file");
  } else {
    Serial.println("Success to open ca file");
  }

  if(netClient->loadCertificate(ca)) {
    Serial.println("loaded");
  } else {
    Serial.println("not loaded");
  }
}


///////////////////////////////
// MQTT common functions
///////////////////////////////
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void startMQTT() {
  mqttClient->begin(CLOUD_IOT_CORE_MQTT_HOST, CLOUD_IOT_CORE_MQTT_PORT, *netClient);
  mqttClient->onMessage(messageReceived);
}

void publishTelemetry() {
///////////////////////////////////////////////////////
char gcpData[256];
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
    String(LastPinStatus.D1Status).toCharArray(dOne, 50);
  }
  if (PinReporting.D2State) {
    String(LastPinStatus.D2Status).toCharArray(dTwo, 50) ;
  }
  if (PinReporting.A0State) {
    String(LastPinStatus.A0Status).toCharArray(aZero, 50) ;
  }
  strcpy(gcpData, "{\"state\":{\"reported\": {\"D0Status\":");
  strcat(gcpData, dZero);
  strcat(gcpData, ", \"D1Status\":");
  strcat(gcpData, dOne);
  strcat(gcpData, ", \"D2Status\":");
  strcat(gcpData, dTwo);
  strcat(gcpData, ", \"A0Status\":");
  strcat(gcpData, aZero);
  strcat(gcpData, ", \"AC\":");
  strcat(gcpData, aa);
  strcat(gcpData, "}}}");
  mqttClient->publish(device->getEventsTopic(), gcpData);
  Serial.println(gcpData);
  delay (3000);
//////////////////////////////////////////////////////
}

// Helper that just sends default sensor
void publishState(String data) {
  mqttClient->publish(device->getStateTopic(), data);
  Serial.println("xxxxxxxx...");
}




 void mqttConnect() {
  Serial.print("\nconnecting...");
  while (!mqttClient->connect(device->getClientId().c_str(), "unused", getJwt().c_str(), false)) {
    Serial.print(".");
    Serial.println(mqttClient->lastError());
    Serial.println(mqttClient->returnCode());
    delay(1000);
  }
  Serial.println("\nconnected!");
  mqttClient->subscribe(device->getConfigTopic());
  mqttClient->subscribe(device->getCommandsTopic());
  Serial.println("GCP Connected");
  //publishState("GCP_Connected");
}


///////////////////////////////
// Orchestrates various methods from preceeding code.
///////////////////////////////


void setupCloudIoT() {
  // Create the device
  device = new CloudIoTCoreDevice(
      project_id, location, registry_id, device_id,
      private_key_str);

  // ESP8266 WiFi setup
  netClient = new WiFiClientSecure();
  //setupWifi();

  // Device/Time OK, ESP8266 refresh JWT
  Serial.println(getJwt());

  // ESP8266 WiFi secure initialization
  setupCert();

  mqttClient = new MQTTClient(512);
  startMQTT(); // Opens connection
}

#endif //__ESP8266_MQTT_H__
