#include "RTC.h"                                        /// Own header file
#include <RTClib.h>                                     /// Adafruit libr DS3231 (RTC_DS3231 class and DateTime)

RTC_DS3231 rtc;                                         /// Communicate DS3231 with I2C

void RTCManager::begin() {
  if (!rtc.begin()) {
    Serial.println("RTC niet gevonden!");
    while(1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC verloren stroom — opnieuw instellen...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void RTCManager::setTime(int jaar, int maand, int dag, int uur, int minuut) {
  DateTime nieuweTijd(jaar, maand, dag, uur, minuut, 0);
  rtc.adjust(nieuweTijd);
}

void RTCManager::start() {                              /// Object start   
  running = true;                                       /// Intern flag stopwatch is running
  startTime = rtc.now();                                /// This is the start
  startMillis = millis();                               /// Saves systemtime in ms 
}

void RTCManager::stopAndReset() {                       /// Object stopAndReset
  running = false;                                      /// Intern flag f clock isn't running
  startTime = DateTime(2000, 1, 1, 0, 0, 0);            /// Local memory startTime to 0
}

DateTime RTCManager::now() {                            ///  
  return running ? rtc.now() : startTime;               /// If clock is running rtc.now time returns else startTime give time stops   
}

unsigned long RTCManager::elapsMillis() {                ///
  return running ? millis() - startMillis : 0;           /// If clock is running then return ms -startms, else 0. 
}

