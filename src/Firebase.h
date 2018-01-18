//
// Created by root on 11/7/17.
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

    void setRequest(const String &path, int len);

public:
    FirebaseClass(const char *host, const char *databaseSecret = "");
    FirebaseClass(const char *host, Oauth *oauth);

    void setInt(const String &path, int value);

    void set(const String &path, const String &value, const String &attr = "value");

    JsonObject &setJson(const String &path, const JsonObject &value);

    String pushJson(const String &path, const JsonObject &value);

    int getInt(const String &path, const String &attr = "value");

    String get(const String &path, const String &attr = "value");

    String getString(const String &path);

    JsonObject &getJson(const String &path);
};


#endif //WEERSTATION_FIREBASE_H
