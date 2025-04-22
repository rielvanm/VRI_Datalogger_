// Includes necessary libraries for GPS, Display, Buttons, RTC, SD, and utility functions
#include <TinyGPS++.h>                               // GPS parsing library (NMEA)
#include "DisplayManager.h"                          // OLED display management
#include "GpsHandler.h"                              // GPS module handler
#include "Utils.h"                                   // Utility functions (e.g., printing digits)
#include "ButtonManager.h"                           // Button input management
#include "SensorTrigger.h"                           // Sensor interrupt handling
#include "RTC.h"                                     // Real-time clock management
#include "SDManager.h"                               // SD card file management
#include "TriggerBuffer.h"                           // Buffer for interrupts/events
#include <SPI.h>
#include <SD.h>
#include "SdFailureHandler.h"                        // SD error handling

// Pin definitions
#define RX0PIN D0                                    // UART RX (GPS data receive)
#define TX0PIN D1                                    // UART TX (GPS data transmit)
#define PIN_START 6
#define PIN_STOP  9
#define PIN_KLOK 7
#define PIN_RETURN 8

// Global states for logging and counters
bool loggingStarted = false;
bool loggingStopped = false;
uint32_t tripCounter = 0;                            // Counts number of completed measurements
volatile uint32_t interruptCounter = 0;              // External interrupt event counter

// Class object instantiations
TriggerBuffer triggerBuffer;
SDManager sd(4);                                     // SD card manager (CS pin = 4)
RTCManager rtcManager;
SensorTrigger sensorTrigger;
ButtonManager buttons;
DisplayManager displayManager;
HardwareSerial GPS(1);                               // GPS on Serial1
const int timeZoneOffset = 1;                        // Local timezone offset (UTC+1)
GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset);

// External bitmap (logo)
extern const unsigned char arduino_icon[];           // Logo bitmap

// Timing variables for button debouncing
unsigned long lastButtonCheck = 0;
const unsigned long buttonInterval = 20;

// Function to calculate the number of days in a month, including leap years
int daysInMonth(int month, int year) {
  if (month == 2) {
    bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    return leapYear ? 29 : 28;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

// Blinks the error message on the display to attract attention
void BlinkingDisplayByError(const String& errorMessage) {
  for (int i = 0; i < 2; i++) {
    displayManager.showMessage(errorMessage.c_str());
    delay(150);
    displayManager.showMessage(" ");
    delay(150);
  }
}

// Arduino initial setup routine
void setup() {                                    
  rtcManager.begin();
  Serial.begin(9600);
  Wire.begin();
  gpsHandler.begin();
  displayManager.begin();
  displayManager.showIntro(arduino_icon);
  buttons.begin();
  sensorTrigger.begin(D3);                         // Sensor interrupt pin

  // Initialize current time and display it
  DateTime now = rtcManager.now();
  displayManager.showDateTimeInfo(now);
  printDateTimeToSerial(now);

  // Initialize button pins
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);

  // Initialize SD card
  if (!sd.begin()) {
    Serial.println("SD initialization failed.");
    displayManager.addUserMessage("Insert SD");
  } else {
    Serial.println("SD initialized successfully.");
    displayManager.addUserMessage("SD ready");
  }
}

// Arduino main loop
void loop() {
  handleSdFailureDuringLogging();
  displayManager.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());

  // Check for sensor interrupts
  if (sensorTrigger.wasTriggered()) {
    DateTime now = rtcManager.now();
    unsigned long ms = rtcManager.elapsMillis() % 1000;
    triggerBuffer.addFromISR(now, ms);
  }

  // Transfer interrupts to processing buffer and process them
  triggerBuffer.transferPending();
  if (triggerBuffer.hasPending()) {
    triggerBuffer.processNext(sd);
  }

  // Button polling (debounced)
  unsigned long now = millis();
  if (now - lastButtonCheck >= buttonInterval) {
    lastButtonCheck = now;

    if (displayManager.getState() == DisplayManager::DisplayState::TimeSet) {
      // Handle button presses for time setting
      ButtonAction clkAction = buttons.readSecondButtons();
      if (clkAction != NONE) {
        switch (clkAction) {
          case PLUS:
            displayManager.timeFields[displayManager.selectedField]++;
            break;

          case MIN:
            displayManager.timeFields[displayManager.selectedField]--;
            break;

          case NEXT:
            displayManager.selectedField = (displayManager.selectedField + 1) % 5;
            break;

          case RETURN: {
            int day = displayManager.timeFields[0];
            int month = displayManager.timeFields[1];
            int year = displayManager.timeFields[2];
            int hour = displayManager.timeFields[3];
            int minute = displayManager.timeFields[4];

            bool valid = true;
            String errorMessage = "";

            if (year < 2020 || year > 2099) { valid = false; errorMessage = "Invalid year"; }
            else if (month < 1 || month > 12) { valid = false; errorMessage = "Invalid month"; }
            else if (day < 1 || day > daysInMonth(month, year)) { valid = false; errorMessage = "Invalid day"; }
            else if (hour < 0 || hour > 23) { valid = false; errorMessage = "Invalid hour"; }
            else if (minute < 0 || minute > 59) { valid = false; errorMessage = "Invalid minute"; }

            if (valid) {
              rtcManager.setTime(year, month, day, hour, minute);
              displayManager.setState(DisplayManager::DisplayState::Menu);
              displayManager.addUserMessage("Time updated");
            } else {
              BlinkingDisplayByError(errorMessage);
            }
            break;
          }
          default:
            break;
        }
      }
    } 
  }
}
