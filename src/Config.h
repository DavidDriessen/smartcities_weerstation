//
// Created by David Driessen on 1/18/18.
//

#ifndef WEERSTATION_CONFIG_H
#define WEERSTATION_CONFIG_H

#define EEPROM_SIZE 512

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#include <ESP32WebServer.h>
#endif

#include "FS.h"
#include "EEPROM.h"

class Config {

public:
/// Run the setup to configure the device
    void runSetup();

/// Save data to the device storage
    bool save(String data[], int len);

/// Load data from the device storage
    bool load(int index, String &data);
};


#endif //WEERSTATION_CONFIG_H
