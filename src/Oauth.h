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
public:
    StaticJsonBuffer<9000> jsonBuffer;
    WiFiClientSecure http_;
    String refreshToken;
    const char *key;
    String access_token;
    String user_id;
public:

    Oauth(){
        refreshToken.reserve(260);
        access_token.reserve(1000);
        user_id.reserve(30);
    }

    void setKey(const char *key) {
        this->key = key;
    }

    void login(const String email, const String password);

    String &getAccessToken(bool New = false);

    String getUserId();
};


#endif //WEERSTATION_OAUTH_H
