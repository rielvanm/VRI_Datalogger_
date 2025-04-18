#ifndef DISPLAYMANAGER_H                                    /// Guard
#define DISPLAYMANAGER_H                                    

#include <Adafruit_SSD1306.h>                               /// Libr SSD1306 OLED 128x64 for Adrduino
#include <TinyGPS++.h> 
#include <RTClib.h>                                         /// Libr for GPS

class DisplayManager {                                      /// Class DisplayManager
public:  
  enum class DisplayState {                                 /// states of the system
  Intro,
  TimeSet,
  Menu,
  Logging,
  Stopped,
  GpsDisplay
};

  DisplayManager();                                         /// Constructor                                       

  void begin();                                             /// Start OLED and erase 
  void showIntro(const unsigned char* logo);                /// Intro with logo
  void updateDisplay(TinyGPSPlus& gps, int timeZoneOffset); /// Freshes screen with actual GPS-data
  void update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow);
  void showMessage(const char* message);                    /// PLACEHOLDER 
  void setState(DisplayState newState);
  void showLoggingScreen(TinyGPSPlus& gps);
  void showSummaryScreen();
  void showInitScreen();
  void addUserMessage(const String& message);
  void showTimeSetScreen(int timeFields[5], int selectedField);
  DisplayState getState() const;

private:
  Adafruit_SSD1306 oled;
  static const int MAX_MESSAGES = 4;
  String userMessages[MAX_MESSAGES];
  bool sdAvailable = false;
  bool lastSdStatus = false;
  bool lastSensorStatus = false;
  unsigned long lastSdCheckTime = 0;
  DisplayState currentState;
  void showMenu(TinyGPSPlus& gps, DateTime rtcNow);
  void showGps(TinyGPSPlus& gps, int timeZoneOffset);
  unsigned long stateStartTime;
  unsigned long lastStatusCheckTime = 0;
  bool writable = false;
  bool interruptDetected = false;
  uint32_t lastInterruptCounter = 0;
  int timeFields[5] = {1, 1, 2025, 0, 0}; // dag, maand, jaar, uur, minuut
  int selectedField = 0;
  bool timeSetConfirmed = false;


  enum class TimeField { DAY, MONTH, YEAR, HOUR, MINUTE };
  TimeField currentField = TimeField::DAY;
  DateTime tempDate = DateTime(2025, 1, 1, 12, 0, 0);
};

#endif
