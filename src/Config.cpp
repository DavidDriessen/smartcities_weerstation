//
// Created by root on 1/18/18.
//

#include "Config.h"


void Config::runSetup() {
    WiFi.softAP("Weerstation");
    Serial.println("");
    Serial.print("Connect to ");
    Serial.println("Weerstation");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

#ifdef ESP8266
    ESP8266WebServer server(80);
#endif
#ifdef ESP32
    ESP32WebServer server(80);
#endif

    server.on("/", [&server]() {
        int numberOfNetworks = WiFi.scanNetworks();
        String networks;
        for (int i = 0; i < numberOfNetworks; ++i) {
            networks += "<option>" + WiFi.SSID(i) + "</option>";
        }
        server.send(200, "text/html", "Weer station setup.<br>"
                                              "<form method='post' action='/setup'>"
                                              "SSID: <select name='ssid'>" + networks +
                                      "</select><br>"
                                              "Password: <input type='password' name='password'><br>"
                                              "Postcode: <input name='postcode'><br>"
                                              "<input type='submit'>"
                                              "</form>");
    });

    server.on("/setup", [&]() {
        String saveData[3] = {server.arg("ssid"), server.arg("password"), server.arg("postcode")};
        save(saveData, 3);
        server.send(200, "text/plain", "Weer station set up.");
        server.handleClient();
        delay(500);
        ESP.restart();
    });

    server.onNotFound([&server]() {
        server.send(404, "text/plain", "");
    });

    server.begin();
    Serial.println("HTTP server started");

    while (true) {
        server.handleClient();
    }
}

bool Config::save(String data[], int len) {
    int count = 0;
    for (int i = 0; i < len; i++) {
        count += data[i].length() + 1;
    }
    if (count < EEPROM_SIZE) {
        if (!EEPROM.begin(EEPROM_SIZE)) {
            Serial.println("failed to initialise EEPROM");
            delay(100);
            ESP.restart();
        }
        int pos = 0;
        for (int i = 0; i < len; i++) {
            for (int j = 0; j < data[i].length(); j++) {
                EEPROM.write(pos++, byte(data[i][j]));
            }
            EEPROM.write(pos++, byte(0));
        }
        for (; pos < EEPROM_SIZE; pos++) {
            EEPROM.write(pos, byte(0));
        }
        EEPROM.commit();
        return true;
    }
    return false;
}

bool Config::load(int index, String &data) {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to initialise EEPROM");
        delay(100);
        ESP.restart();
    }
    int pos = 0;
    char temp = 0;
    for (int i = 0; i <= index; i++) {
        for (; pos < EEPROM_SIZE; pos++) {
            temp = char(EEPROM.read(pos));
            if (temp == 0) {
                pos++;
                break;
            }
            if (i == index) {
                data += temp;
            }
        }
    }
    Serial.println(data);
    return data == "" == 0;
}