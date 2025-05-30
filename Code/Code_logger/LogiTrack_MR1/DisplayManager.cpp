/**
 * @file DisplayManager.cpp
 * @brief Implementation of the DisplayManager class for handling OLED output and display states.
 */

#include "DisplayManager.h"
#include "Utils.h"
#include <RTClib.h>
#include "RTC.h"
#include <SD.h>

/// @brief Width OLED display in Pixels
#define OLED_WIDTH 128

/// @brief Height OLED display in Pixels
#define OLED_HEIGHT 64

/// @brief Chip select pin for the SD card module
#define SD_CS_PIN 4

/// @brief GPIO pin connected to the sensor trigger
#define SENSOR_PIN 5

/// @brief RTC-manager object for access to time and date
extern RTCManager rtcManager;

/// @brief Counts the number of hardware interrupts on the sensor pin
extern volatile uint32_t interruptCounter;

/// @brief Keeps track of the number of detected trips or detection cycles
extern uint32_t tripCounter;

/**
 * @brief Constructs the DisplayManager and initializes display size and SD status.
 */
DisplayManager::DisplayManager()
: oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1), sdAvailable(false) {}

/**
 * @brief Initializes the OLED display and SD card.
 */
void DisplayManager::begin() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oled.println(F("Failed to initialize SSD1306 OLED"));
    while (1);
  }
  oled.clearDisplay();
  oled.display();

  sdAvailable = SD.begin(SD_CS_PIN);
  pinMode(SENSOR_PIN, INPUT);
}

/**
 * @brief Sets a new display state and logs the start time of the state.
 * @param newState The new state to transition into.
 */
void DisplayManager::setState(DisplayState newState) {
  currentState = newState;
  stateStartTime = millis();
}

/**
 * @brief Gets the current display state.
 * @return The current display state.
 */
DisplayManager::DisplayState DisplayManager::getState() const {
  return currentState;
}

/**
 * @brief Displays the date and time on the OLED screen.
 * @param dt The DateTime object containing current date and time.
 */
void DisplayManager::showDateTimeInfo(const DateTime& dt) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "Dat: %04d-%02d-%02d", dt.year(), dt.month(), dt.day());
  addUserMessage(buffer);
  snprintf(buffer, sizeof(buffer), "Tijd: %02d:%02d:%02d", dt.hour(), dt.minute(), dt.second());
  addUserMessage(buffer);
}

/**
 * @brief Main update function, refreshes display based on current state and external statuses.
 * @param gps Reference to TinyGPSPlus object
 * @param timeZoneOffset Timezone offset in hours
 * @param rtcNow Current time from RTC
 */
void DisplayManager::update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow) {
  if (millis() - lastStatusCheckTime > 2000) {
    bool currentSdStatus = SD.begin(SD_CS_PIN);
    writable = false;

    if (currentSdStatus) {
      File testFile = SD.open("/_test.txt", FILE_WRITE);
      if (testFile) {
        testFile.println("test");
        testFile.close();
        SD.remove("/_test.txt");
        writable = true;
      }
    }

    delay(10);

    if (!currentSdStatus && !sdErrorShown) {
      addUserMessage("Plaats SD");
      sdErrorShown = true;
    }

    if (currentSdStatus && sdErrorShown) {
      for (int i = 0; i < MAX_MESSAGES; ++i) {
        if (userMessages[i] == "Plaats SD") {
          userMessages[i] = "";
        }
      }
      addUserMessage("SD gereed");
      sdErrorShown = false;
    }

    if (currentSdStatus != lastSdStatus) {
      lastSdStatus = currentSdStatus;
      sdAvailable = currentSdStatus;
    }

    bool currentSensorStatus = digitalRead(SENSOR_PIN) == HIGH;
    if (currentSensorStatus != lastSensorStatus) {
      lastSensorStatus = currentSensorStatus;
    }

    lastStatusCheckTime = millis();
  }

  static uint32_t lastInterruptCounter = 0;
  if (interruptCounter != lastInterruptCounter) {
    lastIrTriggerTime = millis();
    lastInterruptCounter = interruptCounter;
  }

  interruptDetected = (millis() - lastIrTriggerTime) < 500;

  switch (currentState) {
    case DisplayState::Intro:
      if (millis() - stateStartTime > 1000) {
        setState(DisplayState::Menu);
      }
      break;
    case DisplayState::Menu:
      showMenu(gps, rtcNow);
      break;
    case DisplayState::TimeSet:
      showTimeSetScreen(timeFields, selectedField);
      break;
    case DisplayState::Stopped:
      showSummaryScreen();
      break;
    case DisplayState::GpsDisplay:
      showGps(gps, timeZoneOffset);
      break;
    case DisplayState::Logging:
      break;
  }
}

/**
 * @brief Adds a user message to the message list shown on display.
 * @param message The message to display.
 */
void DisplayManager::addUserMessage(const String& message) {
  for (int i = MAX_MESSAGES - 1; i > 0; i--) {
    userMessages[i] = userMessages[i - 1];
  }
  userMessages[0] = message;
}

/**
 * @brief Shows animated intro with logo sliding across screen.
 * @param logo Pointer to the logo bitmap array.
 */
void DisplayManager::showIntro(const unsigned char* logo) {
  setState(DisplayState::Intro);
  for (int x = 128; x > -80; x--) {
    oled.clearDisplay();
    oled.drawBitmap(0, 0, logo, 128, 64, WHITE);
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 52);
    oled.print(F("v1.0 | LogiTrack MR1"));
    oled.display();
    delay(10);
  }
}

/**
 * @brief Clears all user messages except for "Meting gestart" if present.
 */
void DisplayManager::clearUserInfoExceptStart() {
  bool found = false;
  for (int i = 0; i < MAX_MESSAGES; ++i) {
    if (userMessages[i] == "Meting gestart") {
      found = true;
      break;
    }
  }
  for (int i = 0; i < MAX_MESSAGES; ++i) {
    userMessages[i] = "";
  }
  if (found) {
    userMessages[0] = "Meting gestart";
  }
}

/**
 * @brief Displays the time setting screen with navigation and selection.
 * @param timeFields Array containing day, month, year, hour, and minute.
 * @param selectedField Index of the field currently selected for editing.
 */
void DisplayManager::showTimeSetScreen(int timeFields[5], int selectedField) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println(F("Stel tijd in:"));

  oled.drawLine(0, 54, 128, 54, WHITE);

  oled.setCursor(1, 56);    oled.print(F(" + "));
  oled.setCursor(38, 56);   oled.print(F(" > "));
  oled.setCursor(73, 56);   oled.print(F("OK"));
  oled.setCursor(104, 56);  oled.print(F(" - "));

  oled.setCursor(0, 20);
  for (int i = 0; i < 5; i++) {
    if (i == selectedField)
      oled.setTextColor(BLACK, WHITE);
    else
      oled.setTextColor(WHITE, BLACK);

    if (i == 2)
      oled.print(timeFields[i]);
    else
      printDigits(oled, timeFields[i]);

    if (i == 0 || i == 1) oled.print(F("-"));
    else if (i == 2) oled.print(F(" "));
    else if (i == 3) oled.print(F(":"));
  }

  oled.display();
}

/**
 * @brief Displays the main menu screen with system status and messages.
 * @param gps Reference to TinyGPSPlus object
 * @param rtcNow Current time from RTC
 */
void DisplayManager::showMenu(TinyGPSPlus& gps, DateTime rtcNow) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  oled.drawFastVLine(96, 0, 54, WHITE);
  oled.drawLine(0, 54, 128, 54, WHITE);

  oled.setCursor(1, 56);    oled.print(F("Start"));
  oled.setCursor(38, 56);   oled.print(F("Klok"));
  oled.setCursor(73, 56);   oled.print(F("GPS"));
  oled.setCursor(100, 56);  oled.print(F("Stop"));

  oled.drawFastVLine(32, 54, 10, WHITE);
  oled.drawFastVLine(64, 54, 10, WHITE);
  oled.drawFastVLine(96, 54, 10, WHITE);

  for (int i = 0; i < MAX_MESSAGES; i++) {
    oled.setCursor(0, 10 + i * 10);
    oled.print(userMessages[i]);
  }

  oled.setCursor(100, 0); oled.print(F("SD:"));
  oled.print(sdAvailable ? (writable ? "O" : "X") : "X");

  oled.setCursor(100, 10); oled.print(F("IR:"));
  oled.print(interruptDetected ? "O" : "X");

  oled.setCursor(100, 24); oled.print(F("Rit:"));
  oled.setTextSize(2);
  oled.setCursor(100, 35);
  if (tripCounter < 10) oled.print("0");
  oled.print(tripCounter);

  oled.display();
}

/**
 * @brief Displays a short summary screen showing number of detections.
 */
void DisplayManager::showSummaryScreen() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println(F("Meting gestopt"));
  oled.println();
  oled.print(F("Totaal detecties: "));
  oled.println(interruptCounter);
  oled.display();
}

/**
 * @brief Shows GPS status screen.
 * @param gps Reference to TinyGPSPlus object
 * @param timeZoneOffset Timezone offset to apply to displayed time
 */
void DisplayManager::showGps(TinyGPSPlus& gps, int timeZoneOffset) {
  updateDisplay(gps, timeZoneOffset);
}

/**
 * @brief Displays a temporary message on the screen for one second.
 * @param message The message to show
 */
void DisplayManager::showMessage(const char* message) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println(message);
  oled.display();
  delay(1000);
}

/**
 * @brief Updates the display with current GPS data and time.
 * @param gps Reference to TinyGPSPlus object
 * @param timeZoneOffset Timezone offset for displayed time
 */
void DisplayManager::updateDisplay(TinyGPSPlus& gps, int timeZoneOffset) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);

  if (gps.location.isValid()) {
    oled.print(F("Lat: "));
    oled.println(gps.location.lat(), 8);
    oled.print(F("Lng: "));
    oled.println(gps.location.lng(), 8);
  } else {
    oled.println("Location: INVALID");
  }

  if (gps.date.isValid() && gps.time.isValid()) {
    int hour = gps.time.hour() + timeZoneOffset;
    if (hour >= 24) hour -= 24;

    oled.print(F("Date: "));
    printDigits(oled, gps.date.day());
    oled.print(F("-"));
    printDigits(oled, gps.date.month());
    oled.print(F("-"));
    oled.println(gps.date.year());

    oled.print(F("Time(UTC): "));
    printDigits(oled, hour);
    oled.print(F(":"));
    printDigits(oled, gps.time.minute());
    oled.print(F(":"));
    printDigits(oled, gps.time.second());
  } else {
    oled.println("Time: INVALID");
  }

  oled.display();
}
