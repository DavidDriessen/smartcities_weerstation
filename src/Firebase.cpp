//
// Created by root on 11/7/17.
//

#include "Firebase.h"

void FirebaseClass::begin(const String &host, const String &databaseSecret) {
    this->host = host;
    this->databaseSecret = databaseSecret;
}

void FirebaseClass::begin(const String &host, Oauth *oauth) {
    this->host = host;
    this->oauth = oauth;
}

void FirebaseClass::setInt(const String &path, int value) {
    set(path, String(value));
}

void FirebaseClass::set(const String &path, const String &value, const String &attr) {
    JsonObject &root = getJson(path);
    root[attr] = value;
    setJson(path, root);
}

JsonObject &FirebaseClass::setJson(const String &path, const JsonObject &value) {
    setRequest(path);
    String s;
    value.printTo(s);
    http_.sendRequest("PUT", s);
    http_.end();
}

String FirebaseClass::pushJson(const String &path, const JsonObject &value) {
    setRequest(path);
    String s;
    value.printTo(s);
    http_.POST(s);
    s = http_.getString();
    http_.end();
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
    setRequest(path);
    http_.GET();
    String s = http_.getString();
    http_.end();
    return s;
}

JsonObject &FirebaseClass::getJson(const String &path) {
    jsonBuffer.clear();
    return jsonBuffer.parse(getString(path));
}

void FirebaseClass::setRequest(const String &path) {
    if (databaseSecret.length() > 10) {
        http_.begin(host, 443, "/" + path + ".json?auth=" + databaseSecret, FIREBASE_FINGERPRINT);
    } else if (oauth != nullptr) {
        http_.begin(host, 443, "/" + path + ".json?auth=" + oauth->getAccessToken(), FIREBASE_FINGERPRINT);
    } else {
        http_.begin(host, 443, "/" + path + ".json", FIREBASE_FINGERPRINT);
    }
}

