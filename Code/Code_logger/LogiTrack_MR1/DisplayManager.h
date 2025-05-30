/**
 * @file DisplayManager.h
 * @brief Declaration of the DisplayManager class for controlling the OLED display.
 *
 * This class handles different display states, shows system status, user feedback,
 * GPS and RTC data, and user interaction screens for time configuration.
 */

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <RTClib.h>

/**
 * @class DisplayManager
 * @brief Manages OLED output and display logic for system states and user interaction.
 */
class DisplayManager {
public:
  /**
   * @enum DisplayState
   * @brief Defines possible display states.
   */
  enum class DisplayState {
    Intro,       ///< Intro animation with logo
    TimeSet,     ///< Manual time setting screen
    Menu,        ///< Main menu with status info
    Logging,     ///< Measurement in progress (optional screen)
    Stopped,     ///< Summary after logging
    GpsDisplay   ///< Shows GPS data (lat/lng, time)
  };

  DisplayManager();                          ///< Constructor
  void begin();                              ///< Initialize the OLED display and SD card
  void showIntro(const unsigned char* logo); ///< Intro screen with logo animation
  void updateDisplay(TinyGPSPlus& gps, int timeZoneOffset); ///< Show current GPS data
  void update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow); ///< Main update handler
  void showMessage(const char* message);    ///< Display a short message
  void setState(DisplayState newState);     ///< Set the current display state
  void showLoggingScreen(TinyGPSPlus& gps); ///< Optional: show GPS data while logging
  void showSummaryScreen();                 ///< Show summary screen after logging
  void showInitScreen();                    ///< Optional initialization screen
  void clearUserInfoExceptStart();          ///< Clear all messages except "Meting gestart"
  void addUserMessage(const String& message); ///< Add user feedback to menu
  void showTimeSetScreen(int timeFields[5], int selectedField); ///< Show time-setting interface
  DisplayState getState() const;            ///< Get current display state
  void showDateTimeInfo(const DateTime& dt);///< Show RTC date and time in message list

  int timeFields[5] = {1, 1, 2025, 0, 0};    ///< Time-setting values: day, month, year, hour, minute
  int selectedField = 0;                    ///< Selected index in timeFields array

  bool isSdWritable() const { return writable; }   ///< Check if SD is writable
  bool isSdAvailable() const { return sdAvailable; } ///< Check if SD is inserted

private:
  Adafruit_SSD1306 oled;                    ///< OLED screen interface
  static const int MAX_MESSAGES = 4;        ///< Max number of stored messages
  String userMessages[MAX_MESSAGES];        ///< Scrollable message buffer

  bool lastSdStatus = false;                ///< Last known SD status
  bool lastSensorStatus = false;            ///< Last known sensor status
  unsigned long lastSdCheckTime = 0;        ///< Last SD check timestamp
  DisplayState currentState;                ///< Current display state
  void showMenu(TinyGPSPlus& gps, DateTime rtcNow); ///< Draw the main menu
  void showGps(TinyGPSPlus& gps, int timeZoneOffset); ///< Draw GPS status screen
  unsigned long stateStartTime;             ///< When the current state started
  unsigned long lastStatusCheckTime = 0;    ///< Last global status check timestamp
  bool writable = false;                    ///< True if SD is writable
  bool sdAvailable = false;                 ///< True if SD card is available
  bool interruptDetected = false;           ///< Whether IR trigger was detected
  uint32_t lastInterruptCounter = 0;        ///< Last seen IR counter value
  bool timeSetConfirmed = false;            ///< True if time setting confirmed
  bool sdErrorShown = false;                ///< True if SD error has been displayed
  unsigned long lastIrTriggerTime = 0;      ///< Time of last IR trigger

/**
* @enum TimeField
* @brief Represents editable fields in date/time setting mode.
* 
* This enum is used in the clock setting menu to indicated which part of the data or time
* is currently selected for adjustment by the user.
*/
  enum class TimeField { 
    DAY,    ///< Day of the month (1-31)
    MONTH,  ///< Month of the year (1-12)
    YEAR,   ///< Year (e.g. 2025)
    HOUR,   ///< Hour of the day (0-23)
    MINUTE  ///< Minute of the hour (0-59)
  };
/**
* @brief Currently selected time field in the clock-setting interface.
*
* Used te track wich part datetime is being edited (e.g., day, month, year).
*/
  TimeField currentField = TimeField::DAY; 

  DateTime tempDate = DateTime(2025, 1, 1, 12, 0, 0); ///< Temporary DateTime object used during time setting.
};

#endif // DISPLAYMANAGER_H
