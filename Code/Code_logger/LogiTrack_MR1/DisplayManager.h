#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <RTClib.h>

//
// DisplayManager class
// --------------------
// Responsible for managing display output to a 128x64 OLED screen (SSD1306),
// including screen states (menu, intro, time setting, GPS info, summary),
// user messages, and feedback about system components such as SD card and IR sensor.
//

class DisplayManager {
public:
  // All possible display modes/states
  enum class DisplayState {
    Intro,       // Intro animation with logo
    TimeSet,     // Manual time setting screen
    Menu,        // Main menu with status info
    Logging,     // Measurement in progress (optional screen)
    Stopped,     // Summary after logging
    GpsDisplay   // Shows GPS data (lat/lng, time)
  };

  // Constructor
  DisplayManager();

  // Initialize the OLED display and SD card
  void begin();

  // Intro screen with logo animation
  void showIntro(const unsigned char* logo);

  // Display current GPS data (lat, lng, date, time)
  void updateDisplay(TinyGPSPlus& gps, int timeZoneOffset);

  // Main update loop, responds to state and updates screen accordingly
  void update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow);

  // Simple message display (centered text, 1 second delay)
  void showMessage(const char* message);

  // Set the current display state
  void setState(DisplayState newState);

  // Optional screen: shows GPS data while logging
  void showLoggingScreen(TinyGPSPlus& gps);

  // Show summary screen after measurement is stopped
  void showSummaryScreen();

  // Optional screen shown during initialization
  void showInitScreen();

  // Add a user message to be displayed in the menu (top-down scroll)
  void addUserMessage(const String& message);

  // Display the interface to manually set the time
  void showTimeSetScreen(int timeFields[5], int selectedField);

  // Get the current display state
  DisplayState getState() const;

  // Holds the current time-setting fields: day, month, year, hour, minute
  int timeFields[5] = {1, 1, 2025, 0, 0};

  // Index of currently selected time-setting field
  int selectedField = 0;

  // SD card status helpers
  bool isSdWritable() const { return writable; }
  bool isSdAvailable() const { return sdAvailable; }

  // Add current RTC date and time to user messages
  void showDateTimeInfo(const DateTime& dt);

private:
  // The OLED display instance (SSD1306 over I2C)
  Adafruit_SSD1306 oled;

  // Maximum number of lines for user messages in the menu
  static const int MAX_MESSAGES = 4;

  // Circular buffer of user messages (newest first)
  String userMessages[MAX_MESSAGES];

  // Tracks SD card status for change detection
  bool lastSdStatus = false;

  // Tracks sensor pin status for change detection
  bool lastSensorStatus = false;

  // Time when SD card status was last checked
  unsigned long lastSdCheckTime = 0;

  // The current state of the display
  DisplayState currentState;

  // Show the main menu interface
  void showMenu(TinyGPSPlus& gps, DateTime rtcNow);

  // Show live GPS position and time
  void showGps(TinyGPSPlus& gps, int timeZoneOffset);

  // Timestamp of when the current state started
  unsigned long stateStartTime;

  // Timestamp for the last general status check
  unsigned long lastStatusCheckTime = 0;

  // Indicates whether the SD card is currently writable
  bool writable = false;

  // Indicates whether the SD card is available at all
  bool sdAvailable = false;

  // Set to true if a recent IR interrupt occurred
  bool interruptDetected = false;

  // Last seen value of the interrupt counter
  uint32_t lastInterruptCounter = 0;

  // Set to true when user has confirmed time setting
  bool timeSetConfirmed = false;

  // Timestamp of the last IR trigger
  unsigned long lastIrTriggerTime = 0;

  // Enum for internal tracking of selected time field
  enum class TimeField { DAY, MONTH, YEAR, HOUR, MINUTE };
  TimeField currentField = TimeField::DAY;

  // Temporary DateTime value used during time setting
  DateTime tempDate = DateTime(2025, 1, 1, 12, 0, 0);
};

#endif
