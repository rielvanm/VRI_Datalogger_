#include "DisplayManager.h"
#include "Utils.h"
#include <RTClib.h>
#include "RTC.h"
#include <SD.h>

// OLED display dimensions
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// SD card and sensor pins
#define SD_CS_PIN 4
#define SENSOR_PIN 5

// External objects shared across the system
extern RTCManager rtcManager;
extern volatile uint32_t interruptCounter;
extern uint32_t tripCounter;

// Constructor: initialize display and SD card availability
DisplayManager::DisplayManager()
: oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1), sdAvailable(false) {}

void DisplayManager::begin() {
  // Initialize the OLED screen; halt if it fails
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oled.println(F("Failed to initialize SSD1306 OLED"));
    while (1);
  }

  // Clear screen at startup
  oled.clearDisplay();
  oled.display();

  // Initialize SD card
  sdAvailable = SD.begin(SD_CS_PIN);

  // Configure sensor input pin
  pinMode(SENSOR_PIN, INPUT);
}

// Update the display state and note the time of state change
void DisplayManager::setState(DisplayState newState) {
  currentState = newState;
  stateStartTime = millis();
}

// Return the current display state
DisplayManager::DisplayState DisplayManager::getState() const {
  return currentState;
}

// Display date and time from the RTC or GPS
void DisplayManager::showDateTimeInfo(const DateTime& dt) {
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "Dat: %04d-%02d-%02d", dt.year(), dt.month(), dt.day());
  addUserMessage(buffer);
  snprintf(buffer, sizeof(buffer), "Tijd: %02d:%02d:%02d", dt.hour(), dt.minute(), dt.second());
  addUserMessage(buffer);
}

// Update the display content based on the system state
void DisplayManager::update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow) {
  // Check SD card and sensor status every 2 seconds
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

    delay(10); // short pause before applying changes

  // Show message SD 
  if (!currentSdStatus && !sdErrorShown) {
  addUserMessage("Plaats SD");
  sdErrorShown = true;
  }

  if (currentSdStatus && sdErrorShown) {
  // Old "Plaats SD"-message ereasing
  for (int i = 0; i < MAX_MESSAGES; ++i) {
    if (userMessages[i] == "Plaats SD") {
      userMessages[i] = "";
    }
  }

  addUserMessage("SD gereed");
  sdErrorShown = false;
}

    // Update SD availability flag if it changed
    if (currentSdStatus != lastSdStatus) {
      lastSdStatus = currentSdStatus;
      sdAvailable = currentSdStatus;
    }

    // Update sensor status if it changed
    bool currentSensorStatus = digitalRead(SENSOR_PIN) == HIGH;
    if (currentSensorStatus != lastSensorStatus) {
      lastSensorStatus = currentSensorStatus;
    }

    lastStatusCheckTime = millis();
  }

  // Track when the last interrupt occurred
  static uint32_t lastInterruptCounter = 0;
  if (interruptCounter != lastInterruptCounter) {
    lastIrTriggerTime = millis();
    lastInterruptCounter = interruptCounter;
  }

  // Show IR trigger icon on display for 0,5 second
  interruptDetected = (millis() - lastIrTriggerTime) < 500;

  // Update screen content based on the current state
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

// Scroll new user messages into view, discarding the oldest
void DisplayManager::addUserMessage(const String& message) {
  for (int i = MAX_MESSAGES - 1; i > 0; i--) {
    userMessages[i] = userMessages[i - 1];
  }
  userMessages[0] = message;
}

// Animated intro screen with sliding logo
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

void DisplayManager::clearUserInfoExceptStart() {
  bool found = false;

  // Check on "Meting gestart" 
  for (int i = 0; i < MAX_MESSAGES; ++i) {
    if (userMessages[i] == "Meting gestart") {
      found = true;
      break;
    }
  }

  // Erease all message
  for (int i = 0; i < MAX_MESSAGES; ++i) {
    userMessages[i] = "";
  }

  // "Meting gestart" back on top
  if (found) {
    userMessages[0] = "Meting gestart";
  }

  // Menu or display will by updating after `update()`
}



// Time-setting interface with navigation and selection highlighting
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
      oled.setTextColor(BLACK, WHITE); // Highlight selection
    else
      oled.setTextColor(WHITE, BLACK);

    if (i == 2)
      oled.print(timeFields[i]); // Year
    else
      printDigits(oled, timeFields[i]);

    if (i == 0 || i == 1) oled.print(F("-"));
    else if (i == 2) oled.print(F(" "));
    else if (i == 3) oled.print(F(":"));
  }

  oled.display();
}

// Main menu display with user messages and system status
void DisplayManager::showMenu(TinyGPSPlus& gps, DateTime rtcNow) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  oled.drawFastVLine(96, 0, 54, WHITE);
  oled.drawLine(0, 54, 128, 54, WHITE);

  // Menu button labels
  oled.setCursor(1, 56);    oled.print(F("Start"));
  oled.setCursor(38, 56);   oled.print(F("Klok"));
  oled.setCursor(73, 56);   oled.print(F("GPS"));
  oled.setCursor(100, 56);  oled.print(F("Stop"));

  oled.drawFastVLine(32, 54, 10, WHITE);
  oled.drawFastVLine(64, 54, 10, WHITE);
  oled.drawFastVLine(96, 54, 10, WHITE);

  // Show recent user messages
  for (int i = 0; i < MAX_MESSAGES; i++) {
    oled.setCursor(0, 10 + i * 10);
    oled.print(userMessages[i]);
  }

  // SD card status
  oled.setCursor(100, 0);
  oled.print(F("SD:"));
  oled.setTextColor(WHITE);
  oled.print(sdAvailable ? (writable ? "O" : "X") : "X");

  // IR sensor detection
  oled.setCursor(100, 10);
  oled.print(F("IR:"));
  oled.print(interruptDetected ? "O" : "X");

  // Show trip number
  oled.setCursor(100, 24);
  oled.print(F("Rit:"));
  oled.setTextSize(2);
  oled.setCursor(100, 35);
  if (tripCounter < 10) oled.print("0");
  oled.print(tripCounter);

  oled.display();
}

// Simple summary screen after measurement is stopped
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

// Show GPS data (delegates to updateDisplay)
void DisplayManager::showGps(TinyGPSPlus& gps, int timeZoneOffset) {
  updateDisplay(gps, timeZoneOffset);
}

// Show a short single-line message on screen
void DisplayManager::showMessage(const char* message) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println(message);
  oled.display();
  delay(1000);
}

// Display live GPS position and time information
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
