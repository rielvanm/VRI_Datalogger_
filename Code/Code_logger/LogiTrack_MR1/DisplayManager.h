#ifndef DISPLAYMANAGER_H                                    /// Guard
#define DISPLAYMANAGER_H                                    

#include <Adafruit_SSD1306.h>                               /// Libr SSD1306 OLED 128x64 for Adrduino
#include <TinyGPS++.h>                                      /// Libr for GPS

class DisplayManager {                                      /// Class DisplayManager
public:                                                       
  DisplayManager();                                         /// Constructor                                       
  void begin();                                             /// Start OLED and erase 
  void showIntro(const unsigned char* logo);                /// Intro with logo
  void updateDisplay(TinyGPSPlus& gps, int timeZoneOffset); /// Freshes screen with actual GPS-data
  void update(TinyGPSPlus& gps, int timeZoneOffset);
  void showMessage(const char* message);                    /// PLACEHOLDER 
private:
  enum class DisplayState {                                 /// states of the system
  Intro,
  Menu,
  GpsDisplay
};
  void showMenu(TinyGPSPlus& gps);                                          /// list for the menu
  void showGps(TinyGPSPlus& gps, int timeZoneOffset);       /// signing de location and the time
  Adafruit_SSD1306 oled;                                    /// OLED screen object from adafruit  
  DisplayState currentState;                                ///  
  unsigned long stateStartTime;                             /// 
};

#endif
