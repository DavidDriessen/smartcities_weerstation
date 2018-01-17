//
// Created by root on 11/7/17.
//

#include "Firebase.h"

FirebaseClass::FirebaseClass(const char *host, const char *databaseSecret) :
        host(host), databaseSecret(databaseSecret) {}

FirebaseClass::FirebaseClass(const char *host, Oauth *oauth) :
        host(host), databaseSecret(), oauth(oauth) {}

void FirebaseClass::setInt(const String &path, int value) {
    set(path, String(value));
}

void FirebaseClass::set(const String &path, const String &value, const String &attr) {
    JsonObject &root = getJson(path);
    root[attr] = value;
    setJson(path, root);
}

JsonObject &FirebaseClass::setJson(const String &path, const JsonObject &value) {
//    setRequest(path);
//    String s;
//    value.printTo(s);
//    http_.sendRequest("PUT", s);
//    http_.end();
}

String FirebaseClass::pushJson(const String &path, const JsonObject &value) {
    String s;
    value.printTo(s);
    setRequest(path, s.length());
    http_.print(s);
    while (http_.connected()) {
        String line = http_.readStringUntil('\n');
//        Serial.println(line);
        if (line == "\r") {
//            Serial.println("headers received");
            break;
        }
    }
    http_.readStringUntil('\n');
    s = http_.readStringUntil('}');
    http_.stop();
    s += "}";
    jsonBuffer.clear();
    return jsonBuffer.parse(s)["name"];
}

int FirebaseClass::getInt(const String &path, const String &attr) {
    return getJson(path)[attr];
}

String FirebaseClass::get(const String &path, const String &attr) {
    return getJson(path)[attr];
}

String FirebaseClass::getString(const String &path) {
//    setRequest(path);
//    http_.GET();
//    String s = http_.getString();
//    http_.end();
//    return s;
}

JsonObject &FirebaseClass::getJson(const String &path) {
    jsonBuffer.clear();
    return jsonBuffer.parse(getString(path));
}

void FirebaseClass::setRequest(const String &path, int len) {
    if (String(databaseSecret).length() > 10) {
//        http_.begin(host, 443, "/" + path + ".json?auth=" + databaseSecret, FIREBASE_FINGERPRINT);
    } else if (oauth != nullptr) {
        String &token = oauth->getAccessToken();
        http_.connect(host, 443);
        http_.print("POST https://" + String(host) + "/" + path + ".json?auth=" + token + " HTTP/1.1\r\n"
                "Content-Length: " + len + "\r\n"
                            "Host: " + host + "\r\n"
                            "Content-Type: application/json\r\n\r\n");
    } else {
//        http_.begin(host, 443, "/" + path + ".json", FIREBASE_FINGERPRINT);
    }
}

