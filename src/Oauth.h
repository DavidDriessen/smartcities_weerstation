//
// Created by root on 12/9/17.
//

#ifndef WEERSTATION_OAUTH_H
#define WEERSTATION_OAUTH_H

#include <ArduinoJson.h>

#ifdef ESP8266

#include <WiFiClientSecure.h>

#endif
#ifdef ESP32

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#endif

class Oauth {
    StaticJsonBuffer<9000> jsonBuffer;
    WiFiClientSecure http_;
    String refreshToken;
    String key;
public:
    void setKey(const char *key) {
        this->key = key;
    }

    void login(const String email, const String password);

    String getAccessToken();
};


#endif //WEERSTATION_OAUTH_H
