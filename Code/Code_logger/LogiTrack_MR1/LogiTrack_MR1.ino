#include <TinyGPS++.h>                              // Lyb for translating NMEA data 
#include "DisplayManager.h"                         // Header-file for OLED
#include "GpsHandler.h"                             // GPS-module
#include "Utils.h"                                  // Helpfunction (printing digits)
#include "ButtonManager.h"                          // Buttonpress
#include "SensorTrigger.h"                          // React on trigger interrupt sensor
#include "RTC.h"
#include "SDManager.h"
#include "TriggerBuffer.h"
#include <SPI.h>
#include <SD.h>
#include "SdFailureHandler.h"

// GPS-pinnen
#define RX0PIN D0                                   // Res (UART) data from GPS PIN D0
#define TX0PIN D1                                   // Tran (UART) data to GPS PIN D1
#define PIN_START 6
#define PIN_STOP  9
#define PIN_KLOK 7
#define PIN_RETURN 8 

bool loggingStarted = false;
bool loggingStopped = false;
uint32_t tripCounter = 0; // Telt het aantal volledige metingen
volatile uint32_t interruptCounter = 0;  // externe teller
TriggerBuffer triggerBuffer;
SDManager sd(4);
RTCManager rtcManager;
SensorTrigger sensorTrigger;                        // Object react on interrupt
ButtonManager buttons;                              // Object reading buttons
DisplayManager displayManager;
HardwareSerial GPS(1);                              // Serial 1
//const int timeZoneOffset = 1;                       // Using local time = UTC + 1
//GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset); // GPS-handler pins and timezone

// Externe bitmap (logo)
extern const unsigned char arduino_icon[];          // extern bitmap (array LOGO) 

unsigned long lastButtonCheck = 0;
const unsigned long buttonInterval = 20;

int daysInMonth(int month, int year) {
  if (month == 2) {
    bool leapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    return leapYear ? 29 : 28;
  }
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

void BlinkingDisplayByError(const String& foutmelding) {
  for (int i = 0; i < 2; i++) {
    displayManager.showMessage(foutmelding.c_str());
    delay(150);
    displayManager.showMessage(" ");
    delay(150);
  }
}

void setup() {                                    
  rtcManager.begin();
  Serial.begin(9600);
  Wire.begin();
  gpsHandler.begin();
  displayManager.begin();
  displayManager.showIntro(arduino_icon);
  buttons.begin();
  sensorTrigger.begin(D3);
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

void loop() {
  handleSdFailureDuringLogging();
  displayManager.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());

  if (sensorTrigger.wasTriggered()) {
    DateTime now = rtcManager.now();
    unsigned long ms = rtcManager.elapsMillis() % 1000;
    triggerBuffer.addFromISR(now, ms);
  }

  triggerBuffer.transferPending();
  if (triggerBuffer.hasPending()) {
    triggerBuffer.processNext(sd);
  }

  unsigned long now = millis();
  if (now - lastButtonCheck >= buttonInterval) {
    lastButtonCheck = now;

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
              displayManager.addUserMessage("Detecties: " + String(interruptCounter));
              interruptCounter = 0;
              rtcManager.stopAndReset();
              sd.writeLine("metingen.csv", "");
              displayManager.addUserMessage("Opgeslagen op sd");
              loggingStopped = true;
              loggingStarted = false;
            } else {
              displayManager.addUserMessage("Was al gestopt");
            }
            break;

          default:
            break;
        }
      }
    }
  }
}


