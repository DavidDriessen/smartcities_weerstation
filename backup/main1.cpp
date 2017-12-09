extern "C" {
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include "../src/Firebase.h"
#include <LiquidCrystal_I2C.h>

#define FIREBASE_HOST "smartcities-d2e38.firebaseio.com"
#define FIREBASE_DATABASE_SECRET "f4Lu1azVREDQFqn0Dp0a8NeQerAB8HIitUiEMwJu"
#define WIFI_SSID "OnMarc Guest"
#define WIFI_PASSWORD "zeT3qa0ucb"
//#define WIFI_SSID "Tele2-1"
//#define WIFI_PASSWORD "aa111111"
#define Display_on


FirebaseClass Firebase;
JsonObject *o;
int sensorDataLen = 0;
int sensorData = 0;
bool send = false;
os_timer_t myTimer;

#ifdef Display_on
LiquidCrystal_I2C lcd(0x27, 16, 2);
#endif

void i2cScan() {
    Wire.begin();
    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        } else if (error == 4) {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");

    delay(5000);           // wait 5 seconds for next scan
}

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
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, 1000, true);
}

void setup(void) {
    Serial.begin(115200);
//    Serial.setDebugOutput(1);
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

    Firebase.begin(FIREBASE_HOST, FIREBASE_DATABASE_SECRET);
    o = &Firebase.getJsonObject();
//    user_init();
#ifdef Display_on
    lcd.begin(16, 2);
    lcd.init();
    lcd.backlight();
#endif
}

StaticJsonBuffer<500> jsonBuffer;

void loop(void) {
    if (send) {
        Serial.println("Send");
        String hash = Firebase.pushJson("regen", *o);
        send = false;
    }
    delay(1000);
#ifdef Display_on
    JsonObject &l = Firebase.getJson("lcd");
    l.prettyPrintTo(Serial);
    lcd.clear();
    lcd.setCursor(0, 0);
    l["een"].printTo(lcd);
    lcd.setCursor(0, 1);
    l["twee"].printTo(lcd);
#endif
}
