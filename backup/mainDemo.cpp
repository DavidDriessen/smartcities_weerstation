extern "C" {
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

int sensorDataLen = 0;
int sensorData = 0;
os_timer_t myTimer;
Servo servo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void timerCallback(void *pArg) {
    if (sensorDataLen < 10) {
        sensorData += analogRead(A0);
        sensorDataLen++;
        Serial.println("Running");
    } else {
        lcd.setCursor(0, 1);
        lcd.print(sensorData / 10);
        if (sensorData / 10 > 500) {
            servo.write(0);
        } else{
            servo.write(180);
        }
        sensorDataLen = sensorData = 0;
    }
}

void user_init(void) {
    os_timer_setfn(&myTimer, timerCallback, NULL);
    os_timer_arm(&myTimer, 1000, true);
}

void setup(void) {
    Serial.begin(115200);
    servo.attach(D7);
    lcd.begin(16, 2);
    lcd.init();
    delay(500);
    servo.write(0);
    delay(500);
    servo.write(180);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Regen:");
    user_init();
}

void loop(void) {
}
