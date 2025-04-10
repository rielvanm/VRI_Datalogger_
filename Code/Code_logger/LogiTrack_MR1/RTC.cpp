#include "RTC.h"
#include <RTClib.h>

RTC_DS3231 rtc;

void RTCManager::begin() {
  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while(1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void RTCManager::start() {
  running = true;
  startTime = rtc.now();
  startMillis = millis();
}

void RTCManager::stopAndReset() {
  running = false;
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0));
}

DateTime RTCManager::now() {
  return running ? rtc.now() : startTime;
}

unsigned long RTCManager::elapsMillis() {
  return running ? millis() - startMillis : 0;
}

