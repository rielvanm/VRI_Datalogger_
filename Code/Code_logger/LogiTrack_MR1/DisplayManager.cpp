#include "DisplayManager.h"
#include "Utils.h"
#include <RTClib.h>
#include "RTC.h"
#include <SD.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define SD_CS_PIN 4
#define SENSOR_PIN 5

extern RTCManager rtcManager;
extern volatile uint32_t interruptCounter;
extern uint32_t ritTeller;

DisplayManager::DisplayManager()
: oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1), sdAvailable(false) {}

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

void DisplayManager::setState(DisplayState newState) {
  currentState = newState;
  stateStartTime = millis();
}

DisplayManager::DisplayState DisplayManager::getState() const {
  return currentState;
}

void DisplayManager::update(TinyGPSPlus& gps, int timeZoneOffset, DateTime rtcNow) {
  // Elke 2 seconden SD-kaart opnieuw checken
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
    if (currentSdStatus != lastSdStatus) {
      lastSdStatus = currentSdStatus;
      sdAvailable = currentSdStatus;
    }

    // Sensorstatus checken
    bool currentSensorStatus = digitalRead(SENSOR_PIN) == HIGH;
    if (currentSensorStatus != lastSensorStatus) {
      lastSensorStatus = currentSensorStatus;
    }

    lastStatusCheckTime = millis();
  }

  static uint32_t lastInterruptCounter = 0;
  interruptDetected = (interruptCounter != lastInterruptCounter);
  lastInterruptCounter = interruptCounter;

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

    case DisplayState::Logging:
      showLoggingScreen(gps);
      break;

    case DisplayState::Stopped:
      showSummaryScreen();
      break;

    case DisplayState::GpsDisplay:
      showGps(gps, timeZoneOffset);
      break;
  }
}

void DisplayManager::addUserMessage(const String& message) {
  for (int i = MAX_MESSAGES - 1; i > 0; i--) {
    userMessages[i] = userMessages[i - 1];
  }
  userMessages[0] = message;
}

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

void DisplayManager::showTimeSetScreen(int timeFields[5], int selectedField) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println(F("Stel tijd in:"));

  oled.setCursor(0, 20);
  for (int i = 0; i < 5; i++) {
    if (i == selectedField)
      oled.setTextColor(BLACK, WHITE);
    else
      oled.setTextColor(WHITE, BLACK);

    if (i == 2)
      oled.print(timeFields[i]); // Jaar
    else
      printDigits(oled, timeFields[i]);

    if (i == 0 || i == 1) oled.print(F("-"));
    else if (i == 2) oled.print(F(" "));
    else if (i == 3) oled.print(F(":"));
  }

  oled.setTextColor(WHITE);
  oled.setCursor(0, 45);
  oled.print(F("Save = + / Delete = >"));

  oled.display();
}

void DisplayManager::showMenu(TinyGPSPlus& gps, DateTime rtcNow) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);

  oled.drawFastVLine(96, 0, 54, WHITE);
  oled.drawLine(0, 54, 128, 54, WHITE);

  oled.setCursor(1, 56);    oled.print(F("Start"));
  oled.setCursor(41, 56);   oled.print(F("Uur"));
  oled.setCursor(73, 56);   oled.print(F("Min"));
  oled.setCursor(104, 56);  oled.print(F("Stop"));

  oled.drawFastVLine(32, 54, 10, WHITE);
  oled.drawFastVLine(64, 54, 10, WHITE);
  oled.drawFastVLine(96, 54, 10, WHITE);

  oled.setTextSize(1);
  for (int i = 0; i < MAX_MESSAGES; i++) {
    oled.setCursor(0, 10 + i * 10);
    oled.print(userMessages[i]);
  }

  oled.setCursor(100, 0);
  oled.print(F("SD:"));
  oled.print(sdAvailable ? (writable ? F("I") : F("L")) : F("O"));

  oled.setCursor(100, 10);
  oled.print(F("IR:"));
  oled.print(interruptDetected ? F("I") : F("O"));

  oled.setCursor(100, 24);
  oled.print(F("Rit:"));
  oled.setTextSize(2);
  oled.setCursor(100, 35);
  oled.print(ritTeller);

  oled.display();
}

void DisplayManager::showLoggingScreen(TinyGPSPlus& gps) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println(F("Logging..."));

  oled.print(F("km/h: "));
  oled.print((int)gps.speed.kmph());
  oled.display();
}

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

void DisplayManager::showGps(TinyGPSPlus& gps, int timeZoneOffset) {
  updateDisplay(gps, timeZoneOffset);
}

void DisplayManager::showMessage(const char* message) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);
  oled.println(message);
  oled.display();
  delay(1000);
}

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
