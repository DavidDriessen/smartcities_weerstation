//
// Created by root on 12/14/17.
//

#include "Display.h"

void Display::initDisplay() {
    pinMode(16, OUTPUT);
    digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
    delay(50);
    digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high

    this->init();
    this->flipScreenVertically();
    this->setFont(ArialMT_Plain_10);
    this->setContrast(static_cast<char>(255));

    this->HelTecSplash();
    delay(5000);
    this->WifiSplash();
}

void Display::HelTecSplash() {
    this->clear();
    this->drawXbm(0, 5, HelTec_LOGO_width, HelTec_LOGO_height, HelTec_LOGO_bits);
    this->display();
}

void Display::WifiSplash() {
    this->clear();
    this->drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
    this->display();
}

void Display::showCloud(int16_t x, int16_t y) {
    this->drawXbm(x, y, cloud_width, cloud_height, cloud_bits);
}

void Display::showRain(int16_t x, int16_t y) {
    this->drawXbm(x, y, rain_width, rain_height, rain_bits);
}

void Display::showSun(int16_t x, int16_t y) {
    this->drawXbm(x, y, sun_width, sun_height, sun_bits);
}
