#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <RTClib.h>

class DisplayManager {
public:
  enum class DisplayState {
    Intro,
    TimeSet,
    Menu,
    Logging,
    Stopped,
    GpsDisplay
  };

  DisplayManager();

  void begin();
  void showIntro(const unsigned char* logo);
  void updateDisplay(TinyGPSPlus& gps, int timeZoneOffset);
  void update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow);
  void showMessage(const char* message);
  void setState(DisplayState newState);
  void showLoggingScreen(TinyGPSPlus& gps);
  void showSummaryScreen();
  void showInitScreen();
  void addUserMessage(const String& message);
  void showTimeSetScreen(int timeFields[5], int selectedField);
  DisplayState getState() const;
  int timeFields[5] = {1, 1, 2025, 0, 0};
  int selectedField = 0;
  bool isSdWritable() const { return writable; }
  bool isSdAvailable() const { return sdAvailable; }
  void showDateTimeInfo(const DateTime& dt);

private:
  Adafruit_SSD1306 oled;
  static const int MAX_MESSAGES = 4;
  String userMessages[MAX_MESSAGES];
  bool lastSdStatus = false;
  bool lastSensorStatus = false;
  unsigned long lastSdCheckTime = 0;
  DisplayState currentState;
  void showMenu(TinyGPSPlus& gps, DateTime rtcNow);
  void showGps(TinyGPSPlus& gps, int timeZoneOffset);
  unsigned long stateStartTime;
  unsigned long lastStatusCheckTime = 0;
  bool writable = false;
  bool sdAvailable = false;
  bool interruptDetected = false;
  uint32_t lastInterruptCounter = 0;
  bool timeSetConfirmed = false;
  unsigned long lastIrTriggerTime = 0;

  enum class TimeField { DAY, MONTH, YEAR, HOUR, MINUTE };
  TimeField currentField = TimeField::DAY;
  DateTime tempDate = DateTime(2025, 1, 1, 12, 0, 0);
};

#endif
