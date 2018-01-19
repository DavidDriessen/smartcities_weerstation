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

#define measurementIntervalInMinuts 5

void timerCallback(void *pArg) { // cup / oppervlak = mm neerslag
    (*o)["regenMMPU"] = ((float(gaugeCount) / measurementIntervalInMinuts) * (float(43) / 113)) * 60;
    gaugeCount = 0;
    send = true;
}

void user_init(void) {
#ifdef ESP8266
#define ets_timer_arm(a, b, c) ets_timer_arm_new(a, b, c, 1)
#endif
    ets_timer_setfn(&myTimer, timerCallback, NULL);
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

void setup(void) {
    pinMode(19, INPUT_PULLDOWN);
    pinMode(5, INPUT_PULLDOWN);
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
    (*o)["postcode"] = postcode;
    JsonObject &p = o->createNestedObject("datum");
    p[".sv"] = "timestamp";

    connectToWiFi(ssid, password);
    auth.setKey("AIzaSyBQGAOw3TcQOhHd6ZMnFX8HraBtCsKxB7o");
    auth.login("admin@admin.nl", "adminadmin");
    user_init();
}

void loop(void) {
//    Serial.println(auth.getAccessToken());
    delay(100);
    if (send) {
        Serial.println("Send");
        Firebase.pushJson("sensoren/" + auth.getUserId(), *o);
        send = false;
    }
    if (digitalRead(5) != gaugeStatus) {
        gaugeStatus = (bool) digitalRead(5);
        gaugeCount++;
        Serial.println(gaugeCount);
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
        display.drawString(30, 30,
                           String(((float(gaugeCount) / measurementIntervalInMinuts) * (float(43) / 113)) * 60) +
                           " mm/u");
    } else {
        display.showCloud(5, 15);
    }
    display.display();
#endif
}
