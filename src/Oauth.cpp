//
// Created by root on 12/9/17.
//

#include "Oauth.h"

void Oauth::login(const String email, const String password) {
    JsonObject &root = jsonBuffer.createObject();
    root["email"] = email;
    root["password"] = password;
    root["returnSecureToken"] = true;
    String s;
    root.printTo(s);

    http_.connect("www.googleapis.com", 443);
    http_.print("POST https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=" + key +
                " HTTP/1.1\r\n"
                        "Content-Length: " + s.length() + "\r\n"
                        "Host: www.googleapis.com\r\n"
                        "Content-Type: application/json\r\n\r\n" + s);

    while (http_.connected()) {
        String line = http_.readStringUntil('\n');
//        Serial.println(line);
        if (line == "\r") {
//            Serial.println("headers received");
            break;
        }
    }
    http_.readStringUntil('\n');
    String response = http_.readStringUntil('}');
    response += "}";
    jsonBuffer.clear();
    JsonObject &o = jsonBuffer.parseObject(response);
//        o.prettyPrintTo(Serial);

    http_.stop();
    refreshToken = o["refreshToken"].as<String>();
}

String Oauth::getAccessToken() {
    JsonObject &root = jsonBuffer.createObject();
    root["grant_type"] = "refresh_token";
    root["refresh_token"] = refreshToken;
    String s;
    root.printTo(s);

    http_.connect("securetoken.googleapis.com", 443);
    http_.print("POST https://securetoken.googleapis.com/v1/token?key=" + key +
                " HTTP/1.1\r\n"
                        "Content-Length: " + s.length() + "\r\n"
                        "Host: www.googleapis.com\r\n"
                        "Content-Type: application/json\r\n\r\n" + s);

    while (http_.connected()) {
        String line = http_.readStringUntil('\n');
//        Serial.println(line);
        if (line == "\r") {
//            Serial.println("headers received");
            break;
        }
    }
    http_.readStringUntil('\n');
    String response = http_.readStringUntil('}');
    response += "}";
    jsonBuffer.clear();
    JsonObject &o = jsonBuffer.parseObject(response);
//        o.prettyPrintTo(Serial);

    return o["access_token"].as<String>();
}

void Oauth::save() {
    if (refreshToken.length() < EEPROM_SIZE) {
        if (!EEPROM.begin(EEPROM_SIZE)) {
            Serial.println("failed to initialise EEPROM");
            delay(1000000);
        }
        for (int i = 0; i < refreshToken.length(); i++) {
            EEPROM.write(i, byte(refreshToken[i]));
        }
        EEPROM.commit();
    }
}

bool Oauth::load() {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to initialise EEPROM");
        delay(1000000);
    }
    refreshToken = "";
    for (int i = 0; i < EEPROM_SIZE; i++) {
        refreshToken += char(EEPROM.read(i));
    }
    return getAccessToken().length() > 0;
}
