//
// Created by root on 11/23/17.
//


#define Display_on

#define FIREBASE_HOST "smartcities-d2e38.firebaseio.com"
//#define FIREBASE_DATABASE_SECRET "f4Lu1azVREDQFqn0Dp0a8NeQerAB8HIitUiEMwJu"
#define WIFI_SSID "OnMarc Guest"
#define WIFI_PASSWORD "zeT3qa0ucb"
//#define WIFI_SSID "Tele2-1"
//#define WIFI_PASSWORD "aa111111"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#endif
#ifdef ESP32

#include <WiFi.h>
#include <ESP32WebServer.h>
#include <ESPmDNS.h>

#endif

#ifdef Display_on

#include "Display.h"

#ifdef ESP8266
Display display(D1, D2);
#endif
#ifdef ESP32
Display display(4, 15); // ESP8266 D2,D8
#endif
#endif

#include <ArduinoJson.h>
#include "Firebase.h"
#include "FS.h"

Oauth auth;
FirebaseClass Firebase(FIREBASE_HOST, &auth);
StaticJsonBuffer<50> jsonBuffer;
JsonObject *o;
int sensorDataLen = 0;
int sensorData = 0;
bool send = false;

ETSTimer myTimer;

bool saveWifi(String ssid, String password) {
    if (ssid.length() + password.length() < EEPROM_SIZE - 5) {
        if (!EEPROM.begin(EEPROM_SIZE)) {
            Serial.println("failed to initialise EEPROM");
            delay(100);
            ESP.restart();
        }
        int pos = 0;
        for (int i = 0; i < ssid.length(); i++) {
            EEPROM.write(i, byte(ssid[i]));
            pos++;
        }
        EEPROM.write(pos++, byte(0));
        for (int i = 0; i < password.length(); i++) {
            EEPROM.write(pos++, byte(password[i]));
        }
        EEPROM.write(pos, byte(0));
        EEPROM.commit();
        return true;
    }
    return false;
}

bool loadWifi(String &ssid, String &password) {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to initialise EEPROM");
        delay(100);
        ESP.restart();
    }
    for (int i = 0; i < EEPROM_SIZE; i++) {
        char temp = char(EEPROM.read(i));
        if (temp == 0) {
            for (int j = ++i; j < EEPROM_SIZE; j++) {
                temp = char(EEPROM.read(j));
                if (temp == 0) {
                    break;
                }
                password += temp;
            }
            break;
        }
        ssid += temp;
    }
    return password == "" == 0;
}

void WifiSetup() {
    WiFi.softAP("Weerstation");
    Serial.println("");

#ifdef Display_on
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(32, 20, "Setup");
    display.display();
#endif
    Serial.println("");
    Serial.print("Connect to ");
    Serial.println("Weerstation");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    if (MDNS.begin("Achmea")) {
        Serial.println("MDNS responder started");
    }

    ESP32WebServer server(80);

    server.on("/", [&server]() {
        server.send(200, "text/html", "Weer station setup.<br>"
                "<form method='post' action='/setup'>"
                "ssid: <input name='ssid'><br>"
                "password: <input type='password' name='password'><br>"
                "<input type='submit'>"
                "</form>");
    });

    server.on("/setup", [&server]() {
        saveWifi(server.arg("ssid"), server.arg("password"));
        server.send(200, "text/plain", "Weer station set up.");
        server.handleClient();
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

void authSetup() {
    pinMode(22, INPUT_PULLDOWN);
    if (!auth.load() || digitalRead(22)) {
        Serial.println("Enter email");
        while (Serial.available() == 0) {}
        String email = Serial.readStringUntil('\n');
        Serial.print("Email: ");
        Serial.println(email);
        Serial.println("Enter password");
        while (Serial.available() == 0) {}
        String password = Serial.readStringUntil('\n');
        auth.login(email, password);
        Serial.println(auth.refreshToken);
        Serial.println(auth.getAccessToken());
        if (auth.getAccessToken() != "") {
            Serial.println("Saving");
            auth.save();
        }
    }
}

void timerCallback(void *pArg) {
    if (sensorDataLen < 10) {
        sensorData += analogRead(A0);
        sensorDataLen++;
        Serial.println("Running");
    } else {
        (*o)["neerslag"] = sensorData / 10;
        (*o)["postcode"] = "poscode test";
        JsonObject &p = o->createNestedObject("time");
        p[".sv"] = "timestamp";
        sensorDataLen = sensorData = 0;
        Serial.println("Sending");
        send = true;
    }
}

void user_init(void) {
#ifdef ESP8266
#define ets_timer_arm(a, b, c) ets_timer_arm_new(a, b, c, 1)
#endif
    ets_timer_setfn(&myTimer, timerCallback, NULL);
    ets_timer_arm(&myTimer, 1000, true);
}

void connectToWiFi(String &ssid, String &password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup(void) {
    pinMode(19, INPUT_PULLDOWN);
    Serial.begin(115200);
    Serial.setDebugOutput(false);
#ifdef Display_on
    display.initDisplay();
#endif
    String ssid;
    String password;
    if (!loadWifi(ssid, password) || digitalRead(19)) {
        WifiSetup();
        return;
    }
    Serial.println(ssid);
    Serial.println(password);
    connectToWiFi(ssid, password);
    auth.setKey("AIzaSyBQGAOw3TcQOhHd6ZMnFX8HraBtCsKxB7o");
    auth.login("admin@admin.nl", "adminadmin");
    o = &jsonBuffer.createObject();
    user_init();
}

void loop(void) {
//    Serial.println(auth.getAccessToken());
    delay(5000);
    if (send) {
        Serial.println("Send");
        if (Firebase.pushJson("sensoren/" + auth.getUserId(), *o) == "") {

        }
        send = false;
    }
#ifdef Display_on
//    JsonObject &l = Firebase.getJson("users/MnJuo2LMGHTWahq3Hz5e783Mqr83");
//    l.prettyPrintTo(Serial);
    display.clear();
    display.showCloud(5, 5);
    display.showRain(30, 5);
    display.showSun(55, 5);
    display.setFont(ArialMT_Plain_10);
    display.drawString(64, 15, "Demo text.");
    display.display();
#endif
    delay(1000);
}
