/**
 * @file LogiTrack_MR1.ino
 * @brief Main application file for the LogiTrack data logger.
 * 
 * This file initializes the system components and manages the runtime logic,
 * including GPS reading, RTC handling, button interactions, and SD card logging.
 */

#include <TinyGPS++.h>              ///< Library for parsing NMEA GPS data
#include "DisplayManager.h"         ///< Manages OLED display output
#include "GpsHandler.h"             ///< Handles GPS communication
#include "Utils.h"                  ///< Helper functions for formatting output
#include "ButtonManager.h"          ///< Reads and processes button presses
#include "SensorTrigger.h"          ///< Handles trigger sensor interrupts
#include "RTC.h"                    ///< Manages RTC timekeeping
#include "SDManager.h"              ///< Handles SD card read/write
#include "TriggerBuffer.h"          ///< Buffers sensor events with timestamps
#include <SPI.h>
#include <SD.h>
#include "SdFailureHandler.h"      ///< Handles SD card errors

// --- GPS and Button Pin Definitions ---
#define RX0PIN D0                   ///< GPS RX pin
#define TX0PIN D1                   ///< GPS TX pin
#define PIN_START 6                ///< Start button pin
#define PIN_STOP  9                ///< Stop button pin
#define PIN_KLOK 7                 ///< Clock set button pin
#define PIN_RETURN 8               ///< Return button pin

// --- Global Variables ---
bool loggingStarted = false;                     ///< Indicates if logging is active
bool loggingStopped = false;                     ///< Indicates if logging is stopped
uint32_t tripCounter = 0;                        ///< Counts full logging sessions
volatile uint32_t interruptCounter = 0;          ///< Interrupt detection counter
TriggerBuffer triggerBuffer;                    ///< Buffer for sensor triggers
SDManager sd(4);                                 ///< SD card interface
RTCManager rtcManager;                           ///< RTC timekeeper
SensorTrigger sensorTrigger;                     ///< Manages sensor interrupts
ButtonManager buttons;                           ///< Button input handler
DisplayManager displayManager;                   ///< Manages display output
HardwareSerial GPS(1);                           ///< UART interface for GPS
const int timeZoneOffset = 1;                    ///< Local time offset from UTC
GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset); ///< GPS handler object
extern const unsigned char arduino_icon[];       ///< Startup logo bitmap

unsigned long lastButtonCheck = 0;               ///< Timestamp for last button poll
const unsigned long buttonInterval = 20;         ///< Minimum interval between button polls

/**
 * @brief Returns number of days in a given month/year.
 * @param month The month (1-12)
 * @param year The full year (e.g., 2024)
 * @return Number of days in the given month
 */
int daysInMonth(int month, int year) {
  if (month == 2) {
    bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    return leapYear ? 29 : 28;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

/**
 * @brief Displays an error message in a blinking fashion.
 * @param foutmelding The message to display.
 */
void BlinkingDisplayByError(const String& foutmelding) {
  for (int i = 0; i < 2; i++) {
    displayManager.showMessage(foutmelding.c_str());
    delay(150);
    displayManager.showMessage(" ");
    delay(150);
  }
}

/**
 * @brief Initializes all subsystems.
 * @details
 * This function sets up the RTC, serial communication, GPS, buttons, display,
 * sensor triggers, and SD card. It also prints the current time to the serial monitor.
 */
void setup() {
  rtcManager.begin();
  Serial.begin(9600);
  Wire.begin();
  gpsHandler.begin();
  displayManager.begin();
  displayManager.showIntro(arduino_icon);
  buttons.begin();
  sensorTrigger.begin(D3);
  sensorTrigger.setTriggerBuffer(&triggerBuffer);

  DateTime nu = rtcManager.now();
  displayManager.showDateTimeInfo(nu);
  printDateTimeToSerial(nu);

  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);

  if (!sd.begin()) {
    Serial.println("SD-kaart initialisatie mislukt.");
    displayManager.addUserMessage("Plaats SD");
  } else {
    Serial.println("SD-kaart succesvol geïnitialiseerd.");
    displayManager.addUserMessage("SD gereed");
  }
}
/**
 * @brief Main program loop.
 * @details
 * Performs the following:
 * - Updates the display with GPS and RTC data
 * - Buffers sensor interrupts with timestamps
 * - Processes buttons for time-setting and menu navigation
 * - Starts/stops logging sessions
 * - Writes to the SD card if applicable
 */
void loop() {
  handleSdFailureDuringLogging();///< Check and handle SD card errors

  // Update display with current GPS and time info
  displayManager.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());

  // Check for sensor trigger and buffer timestamp
if (loggingStarted) {
  triggerBuffer.transferPending();
  while (triggerBuffer.hasPending()) {
    triggerBuffer.processNext(sd);
  }
}

  // Poll buttons at defined interval
  unsigned long now = millis();
  if (now - lastButtonCheck >= buttonInterval) {
    lastButtonCheck = now;

    // Handle clock-setting mode
    if (displayManager.getState() == DisplayManager::DisplayState::TimeSet) {
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

            bool geldig = true;
            String foutmelding = "";

            if (year < 2020 || year > 2099) { geldig = false; foutmelding = "Jaar ongeldig"; }
            else if (month < 1 || month > 12) { geldig = false; foutmelding = "Maand ongeldig"; }
            else if (day < 1 || day > daysInMonth(month, year)) { geldig = false; foutmelding = "Dag ongeldig"; }
            else if (hour < 0 || hour > 23) { geldig = false; foutmelding = "Uur ongeldig"; }
            else if (minute < 0 || minute > 59) { geldig = false; foutmelding = "Minuut ongeldig"; }

            if (geldig) {
              rtcManager.setTime(year, month, day, hour, minute);
              displayManager.setState(DisplayManager::DisplayState::Menu);
              displayManager.addUserMessage("Tijd aangepast");
            } else {
              BlinkingDisplayByError(foutmelding);
            }
            break;
          }

          default:
            break;
        }
      }
    } else {
       // Handle main menu button actions
      ButtonAction action = buttons.readButtons();
      if (action != NONE) {
        switch (action) {
          case START:
            if (!displayManager.isSdAvailable() || !displayManager.isSdWritable()) {
              displayManager.addUserMessage("Plaats SD-kaart");
              break;
            }

            if (!loggingStarted) {
              displayManager.clearUserInfoExceptStart();
              displayManager.addUserMessage("Meting gestart");
              rtcManager.start();
              tripCounter++;
              triggerBuffer.resetElapsed();
              sensorTrigger.arm();
              loggingStarted = true;
              loggingStopped = false;

              DateTime now = rtcManager.now();
              char header[64];
              snprintf(header, sizeof(header), "Rit %d, %d-%02d-%02d, %02d:%02d:%02d",
                       tripCounter,
                       now.year(), now.month(), now.day(),
                       now.hour(), now.minute(), now.second());

              Serial.println(header);
              sd.writeLine("metingen.csv", header);
            } else {
              displayManager.addUserMessage("Meting loopt");
            }
            break;

          case KLOK:
            if (loggingStarted) {
              displayManager.addUserMessage("Niet beschikbaar");
            } else {
              DateTime nu = rtcManager.now();
              displayManager.timeFields[0] = nu.day();
              displayManager.timeFields[1] = nu.month();
              displayManager.timeFields[2] = nu.year();
              displayManager.timeFields[3] = nu.hour();
              displayManager.timeFields[4] = nu.minute();

              displayManager.setState(DisplayManager::DisplayState::TimeSet);
            }
            break;

          case SHOW_GPS:
            if (loggingStarted) {
              displayManager.addUserMessage("Niet beschikbaar");
            } else {
              displayManager.addUserMessage("Nog geen GPS");
            }
            break;

  case STOP:
  if (!loggingStopped) {
    displayManager.addUserMessage("Meting gestopt");

    sensorTrigger.disarm();

    if (loggingStarted) {
      triggerBuffer.transferPending();
      while (triggerBuffer.hasPending()) {
        triggerBuffer.processNext(sd);
      }
    }

    displayManager.addUserMessage("Detecties: " + String(interruptCounter));
    interruptCounter = 0;

    sd.writeLine("metingen.csv", "");

    rtcManager.stopAndReset();

    displayManager.addUserMessage("Opgeslagen op sd");
    loggingStopped = true;
    loggingStarted = false;

  } else {
    displayManager.addUserMessage("Was al gestopt");
  }
  break;
        }
      }
    }
  }
}


