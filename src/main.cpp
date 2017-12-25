//
// Created by root on 11/23/17.
//


#define Display_on

#define FIREBASE_HOST "smartcities-d2e38.firebaseio.com"
#define FIREBASE_DATABASE_SECRET "f4Lu1azVREDQFqn0Dp0a8NeQerAB8HIitUiEMwJu"
//#define WIFI_SSID "OnMarc Guest"
//#define WIFI_PASSWORD "zeT3qa0ucb"
#define WIFI_SSID "Tele2-1"
#define WIFI_PASSWORD "aa111111"

#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32

#include <WiFi.h>

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

FirebaseClass Firebase;
Oauth auth;
StaticJsonBuffer<500> jsonBuffer;
JsonObject *o;
int sensorDataLen = 0;
int sensorData = 0;
bool send = false;

ETSTimer myTimer;

void timerCallback(void *pArg) {
    if (sensorDataLen < 10) {
        sensorData += analogRead(A0);
        sensorDataLen++;
        Serial.println("Running");
    } else {
        (*o)["regen"] = sensorData / 10;
        (*o)["name"] = "YOLO";
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

void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
    Serial.begin(115200);
//    Serial.setDebugOutput(1);
#ifdef Display_on
    display.initDisplay();
#endif
    connectToWiFi();
    Firebase.begin(FIREBASE_HOST, FIREBASE_DATABASE_SECRET);
    auth.setKey("AIzaSyBQGAOw3TcQOhHd6ZMnFX8HraBtCsKxB7o");
    if (!auth.load()) {
//        auth.login("daviddriessen@live.nl", "7k69i5SqLfSa");
        Serial.println("Enter email");
        while(Serial.available() == 0){}
        String email = Serial.readStringUntil('\n');
        Serial.print("Email: ");
        Serial.println(email);
        Serial.println("Enter password");
        while(Serial.available() == 0){}
        String password = Serial.readStringUntil('\n');
        auth.login(email, password);
    }
    o = &jsonBuffer.createObject();
//    user_init();
}

void loop(void) {
    Serial.println(auth.getAccessToken());
    delay(5000);
    if (send) {
        Serial.println("Send");
        String hash = Firebase.pushJson("regen", *o);
        send = false;
    }
#ifdef Display_on
//    JsonObject &l = Firebase.getJson("users/MnJuo2LMGHTWahq3Hz5e783Mqr83");
//    l.prettyPrintTo(Serial);
    display.clear();
    display.showCloud(5, 5);
    display.showRain(30, 5);
    display.showSun(55, 5);
    display.drawString(64, 15, "Demo text.");
    display.display();
#endif
    delay(1000);
}
