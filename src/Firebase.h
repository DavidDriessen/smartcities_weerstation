//
// Created by David Driessen on 11/7/17.
//

#ifndef WEERSTATION_FIREBASE_H
#define WEERSTATION_FIREBASE_H

#include <ArduinoJson.h>
#include "Oauth.h"

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif
#ifdef ESP32
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#endif

class FirebaseClass {
    StaticJsonBuffer<500> jsonBuffer;
    WiFiClientSecure http_;
    const char *host;
    const char *databaseSecret;
    Oauth *oauth = nullptr;

/// Setup the connection with the database
    void setRequest(const String &path, int len);

public:
    explicit FirebaseClass(const char *host, const char *databaseSecret = "");
    FirebaseClass(const char *host, Oauth *oauth);

/// Push a new record to the database
    String pushJson(const String &path, const JsonObject &value);
};


#endif //WEERSTATION_FIREBASE_H
