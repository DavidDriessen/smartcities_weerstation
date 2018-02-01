//
// Created by David Driessen on 11/7/17.
//

#include "Firebase.h"

FirebaseClass::FirebaseClass(const char *host, const char *databaseSecret) :
        host(host), databaseSecret(databaseSecret) {}

FirebaseClass::FirebaseClass(const char *host, Oauth *oauth) :
        host(host), databaseSecret(), oauth(oauth) {}

String FirebaseClass::pushJson(const String &path, const JsonObject &value) {
    String s;
    value.printTo(s);
    setRequest(path, s.length());
    http_.print(s);
    while (http_.connected()) {
        String line = http_.readStringUntil('\n');
        if (line == "\r") {
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

void FirebaseClass::setRequest(const String &path, int len) {
    if (String(databaseSecret).length() > 10) {
        http_.print("POST https://" + String(host) + "/" + path + ".json?auth=" + databaseSecret + " HTTP/1.1\r\n"
                "Content-Length: " + len + "\r\n"
                            "Host: " + host + "\r\n"
                            "Content-Type: application/json\r\n\r\n");
    } else if (oauth != nullptr) {
        String &token = oauth->getAccessToken();
        http_.connect(host, 443);
        http_.print("POST https://" + String(host) + "/" + path + ".json?auth=" + token + " HTTP/1.1\r\n"
                "Content-Length: " + len + "\r\n"
                            "Host: " + host + "\r\n"
                            "Content-Type: application/json\r\n\r\n");
    }
}

