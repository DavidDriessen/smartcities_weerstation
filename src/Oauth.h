//
// Created by David Driessen on 12/9/17.
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
    const char *key{};
    String access_token;
    String user_id;
public:

    Oauth();

/// Set the application key to get access to
    void setKey(const char *key) {
        this->key = key;
    }

/// Login with user to get access
    void login(String email, String password);

/// Get the access token of the user
    String &getAccessToken(bool New = false);

/// Get the id of the user
    String getUserId();
};


#endif //WEERSTATION_OAUTH_H
