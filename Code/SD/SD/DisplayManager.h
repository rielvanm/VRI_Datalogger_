#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>

class DisplayManager {
public:
  DisplayManager();
  void begin();
  void showIntro(const unsigned char* logo);
  void updateDisplay(const TinyGPSPlus& gps, int timeZoneOffset);
private:
  Adafruit_SSD1306 oled;
  void printDigits(int digits);
};

#endif
