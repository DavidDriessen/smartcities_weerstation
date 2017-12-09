//
// Created by root on 11/7/17.
//

#ifndef WEERSTATION_FIREBASE_H
#define WEERSTATION_FIREBASE_H

#include <ArduinoJson.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif
#ifdef ESP32
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#endif

#define FIREBASE_FINGERPRINT "B8:4F:40:70:0C:63:90:E0:07:E8:7D:BD:B4:11:D0:4A:EA:9C:90:F6"

class FirebaseClass {
    StaticJsonBuffer<500> jsonBuffer;
    HTTPClient http_;
    String host;
    String databaseSecret;

    void setRequest(const String &path);

public:
    void begin(const String &host, const String &databaseSecret = "");

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
