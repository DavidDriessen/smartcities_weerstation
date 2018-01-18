//
// Created by root on 1/18/18.
//

#ifndef WEERSTATION_CONFIG_H
#define WEERSTATION_CONFIG_H

#define EEPROM_SIZE 512


#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif
#ifdef ESP32

#include "WiFi.h"
#include <ESP32WebServer.h>

#endif

#include "FS.h"
#include "EEPROM.h"

class Config {

public:
    void runSetup();

    bool save(String data[], int len);

    bool load(int index, String &data);
};


#endif //WEERSTATION_CONFIG_H
