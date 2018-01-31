//
// Created by root on 11/23/17.
//


#define Display_on

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

#ifndef INPUT_PULLDOWN
#define INPUT_PULLDOWN INPUT
#endif

#include <ArduinoJson.h>
#include "Firebase.h"
#include "Config.h"

Oauth auth;
FirebaseClass Firebase("smartcities-d2e38.firebaseio.com", &auth);
StaticJsonBuffer<200> jsonBuffer;
JsonObject *o;
Config con;
int gaugeCount = 0;
bool gaugeStatus = false;
bool send = false;

ETSTimer myTimer;

#define measurementIntervalInMinuts 1

float calcRainFall(float count, boolean interval = true) {
    const float mmPgauge = 43.f / 113.f;
    if(interval) {
        const float measurementIntervalToHoures = 60.f / measurementIntervalInMinuts;
        return count * measurementIntervalToHoures * mmPgauge;
    }
    return count * mmPgauge;
}

void timerCallback(void *pArg) { // cup / oppervlak = mm neerslag
    (*o)["rainMMPH"] = calcRainFall(gaugeCount);
    gaugeCount = 0;
    send = true;
}

void user_init() {
#ifdef ESP8266
#define ets_timer_arm(a, b, c) ets_timer_arm_new(a, b, c, 1)
#endif
    ets_timer_setfn(&myTimer, timerCallback, nullptr);
    ets_timer_arm(&myTimer, measurementIntervalInMinuts * 60000, true);
}

void connectToWiFi(String &ssid, String &password) {
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    pinMode(19, INPUT_PULLDOWN);
    pinMode(5, INPUT_PULLDOWN);
    pinMode(17, INPUT_PULLDOWN);
    Serial.begin(115200);
    Serial.setDebugOutput(false);
#ifdef Display_on
    display.initDisplay();
#endif
    String ssid;
    String password;
    if (!con.load(0, ssid) || !con.load(1, password) || digitalRead(19)) {
#ifdef Display_on
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(32, 20, "Setup");
        display.display();
#endif
        con.runSetup();
    }

    String postcode;
    con.load(2, postcode);
    o = &jsonBuffer.createObject();
    (*o)["zipCode"] = postcode;
    JsonObject &p = o->createNestedObject("date");
    p[".sv"] = "timestamp";

    connectToWiFi(ssid, password);
    auth.setKey("AIzaSyBQGAOw3TcQOhHd6ZMnFX8HraBtCsKxB7o");
    auth.login("admin@admin.nl", "adminadmin");
    user_init();
}

void loop() {
//    Serial.println(auth.getAccessToken());
    delay(100);
    if (send) {
        Serial.println("Send");
        Firebase.pushJson("sensors/" + auth.getUserId(), *o);
        send = false;
    }
    int left = digitalRead(5);
    int right = digitalRead(17);
//    Serial.println(String(left) + " " + String(right));
    if ((left + right) == 1) {
        if (right != gaugeStatus) {
            gaugeStatus = (bool) right;
            gaugeCount++;
            Serial.println(gaugeCount);
        }
    }
#ifdef Display_on
//    JsonObject &l = Firebase.getJson("users/MnJuo2LMGHTWahq3Hz5e783Mqr83");
//    l.prettyPrintTo(Serial);
    display.clear();
    display.showSun(5, 5);
    display.setFont(ArialMT_Plain_10);
    if (gaugeCount > 0) {
        display.showRain(5, 15);
        display.drawString(30, 10, "Het regent.");
        display.drawString(30, 30, String(calcRainFall(gaugeCount, false)) + " mm");
    } else {
        display.showCloud(5, 15);
    }
    display.display();
#endif
}
