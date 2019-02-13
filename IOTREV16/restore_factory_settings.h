#ifndef RESTORE_FACTORY_SETTINGS_H
#define RESTORE_FACTORY_SETTINGS_H

#include <EEPROM.h>
#include "EEPROMAnything.h"


void restore_factory_settings() {

    for (int i = 0; i < 512; i++){
          EEPROM.write(i, NULL);
    } EEPROM.commit(); //saves changes to flash

    struct Reporting PinReporting;
    struct ESPPins PinSet;
    bool DweetData = true;
    
    PinSet.D3 = false;
    PinSet.D4 = false;
    PinSet.D5 = false;
    PinSet.A6 = false;
    
    PinReporting.frequency = 5;
    PinReporting.D0State = false;
    PinReporting.D1State = false;
    PinReporting.D2State = false;
    PinReporting.A0State = false;
    PinReporting.A0Scale = 50;
    PinReporting.A0Threshold = 3;
    sprintf(PinReporting.D0Name, "D0Status");
    sprintf(PinReporting.D1Name, "D1Status");
    sprintf(PinReporting.D2Name, "D2Status");
    sprintf(PinReporting.A0Name, "A0Status");
    sprintf(PinReporting.messageID, "messageID");

    char IOT_CONFIG_CONNECTION_STRING[200]; 
    char aws_endpoint[];
    char aws_key[];
    char aws_secret[];
    char gcp_regId[];
    char gcp_devId[];
    
    sprintf(IOT_CONFIG_CONNECTION_STRING, "NOT_SET");

    EEPROM_writeAnything(0, PinReporting);
    EEPROM_writeAnything(128, PinSet);
    EEPROM_writeAnything(150, DweetData);
    EEPROM_writeAnything(155, "YES");
    EEPROM_writeAnything(163,IOT_CONFIG_CONNECTION_STRING); //will be approx 190 char
    EEPROM_writeAnything(353, aws_endpoint);// will be approx 55 char 
    EEPROM_writeAnything(408, aws_key); // will be approx 25 char
    EEPROM_writeAnything(433, aws_secret); // will be approx 45 char
    EEPROM_writeAnything(478, gcp_regId); // must be less then 15 char
    EEPROM_writeAnything(495, gcp_devId); // must be less then 15 char

    for (int i = 0; i < 512; i++){
     char temp = EEPROM.read(i);
     Serial.print(temp);
    }

Serial.println("");
Serial.println("");
Serial.println("Factory Settings Restored");


}

#endif /* RESTORE_FACTORY_SETTINGS_H */
