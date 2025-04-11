#include "RTC.h"                                        /// Own header file
#include <RTClib.h>                                     /// Adafruit libr DS3231 (RTC_DS3231 class and DateTime)

RTC_DS3231 rtc;                                         /// Communicate DS3231 with I2C

void RTCManager::begin() {                              /// Object in class RTCManager  
  if (!rtc.begin()) {                                   /// checks RTC ready on I2C
    Serial.println("RTC not found!");
    while(1);                                           /// Endless loop 
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));       /// Systemtime  
}

void RTCManager::start() {                              /// Object start   
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));            /// Reset clock to 0
  delay(10);                                            /// Delay for stablisation
  running = true;                                       /// Intern flag stopwatch is running
  startTime = rtc.now();                                /// This is the start
  startMillis = millis();                               /// Saves systemtime in ms 
}

void RTCManager::stopAndReset() {                       /// Object stopAndReset
  running = false;                                      /// Intern flag f clock isn't running
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));            /// Set DS3231 back to 0
  startTime = DateTime(2000, 1, 1, 0, 0, 0);            /// Local memory startTime to 0
}

DateTime RTCManager::now() {                            ///  
  return running ? rtc.now() : startTime;               ///   
}

unsigned long RTCManager::elapsMillis() {                ///
  return running ? millis() - startMillis : 0;           /// 
}

