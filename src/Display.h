//
// Created by root on 12/14/17.
//

#ifndef WEERSTATION_DISPLAY_H
#define WEERSTATION_DISPLAY_H


#include "SSD1306.h"
#include "images.h"

class Display : public SSD1306 {
public:
    Display(uint8_t _sda, uint8_t _scl) : SSD1306(0x3c, _sda, _scl) {}

///  Initiate the display
    void initDisplay();

///  Display the heltec logo on the screen
    void HelTecSplash();

///  Display the wifi logo on the screen
    void WifiSplash();

///  Place a cloud icon on the screen
    void showCloud(int16_t x, int16_t y);

///  Place a rain icon on the screen
    void showRain(int16_t x, int16_t y);

///  Place a sun icon on the screen
    void showSun(int16_t x, int16_t y);
};


#endif //WEERSTATION_DISPLAY_H
