//
// Created by root on 12/9/17.
//

#ifndef WEERSTATION_OAUTH_H
#define WEERSTATION_OAUTH_H

#include <ArduinoJson.h>

#ifdef ESP8266
#include <ESP8266HTTPClient.h>
#endif
#ifdef ESP32
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#endif


#define GOOGLE_API_FINGERPRINT "BC:2D:0B:2A:E3:8F:CA:A1:EB:15:DD:BC:E3:B6:1E:46:C6:E6:A6:80"

class Oauth {
    StaticJsonBuffer<500> jsonBuffer;
    HTTPClient http_;
    String refreshToken;
    String key;
public:
    Oauth(const char *key) : key(key) {

    }

    void login(const String email, const String password) {
        http_.begin("www.googleapis.com", 443, "/identitytoolkit/v3/relyingparty/verifyPasswordV?key=" + key,
                    GOOGLE_API_FINGERPRINT);
        http_.addHeader("Content-Type", "application/json");
        JsonObject &root = jsonBuffer.createObject();
        root["email"] = email;
        root["password"] = password;
        root["returnSecureToken"] = true;
        String s;
        root.printTo(s);
        http_.POST(s);
        s = http_.getString();
        http_.end();
        jsonBuffer.clear();
        refreshToken = jsonBuffer.parse(s)["refreshToken"].as<String>();
    }

    String getAccessToken() {
        http_.begin("securetoken.googleapis.com", 443, "/v1/token?key=",
                    GOOGLE_API_FINGERPRINT);
        http_.addHeader("Content-Type", "application/json");
        JsonObject &root = jsonBuffer.createObject();
        root["grant_type"] = "refresh_token";
        root["refresh_token"] = refreshToken;
        String s;
        root.printTo(s);
        http_.POST(s);
        s = http_.getString();
        http_.end();
        jsonBuffer.clear();
        return jsonBuffer.parse(s)["access_token"];
    }
};


#endif //WEERSTATION_OAUTH_H
