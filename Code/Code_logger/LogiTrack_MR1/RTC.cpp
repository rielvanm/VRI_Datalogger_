#include "RTC.h"             // Own header file for RTCManager
#include <RTClib.h>          // Adafruit RTClib (provides RTC_DS3231 and DateTime)

// Internal RTC object (connected via I2C)
RTC_DS3231 rtc;

/**
 * @brief Initializes the DS3231 RTC module.
 * If the RTC is not found, the program will stop.
 * Also checks for power loss and notifies the user.
 */
void RTCManager::begin() {
  if (!rtc.begin()) {
    Serial.println("RTC not found!");
    while (1); // Halt execution if RTC is missing
  }

  // If RTC lost power, the time may be invalid and needs to be set manually
  if (rtc.lostPower()) {
    Serial.println("RTC lost power. Time must be set manually.");
    // You could set a flag here to prompt user interaction
  }
}


void RTCManager::setTime(int year, int month, int day, int hour, int minute) {
  DateTime newTime(year, month, day, hour, minute, 0);
  rtc.adjust(newTime);
}

/**
 * @brief Starts the timer logic by saving current time from RTC and system clock.
 */
void RTCManager::start() {
  running = true;
  startTime = rtc.now();        // Capture real time at start
  startMillis = millis();       // Save system uptime at start
}

/**
 * @brief Stops the timer and resets the stored start time.
 * Sets the internal running flag to false.
 */
void RTCManager::stopAndReset() {
  running = false;
  startTime = DateTime(2000, 1, 1, 0, 0, 0); // Reset to a neutral time (used as placeholder)
}

/**
 * @brief Returns current time from RTC.
 * Note: In current implementation, always returns rtc.now().
 * 
 * @return DateTime representing the current time.
 */
DateTime RTCManager::now() {
  return rtc.now();  // You could return startTime if running == false, for frozen-time logic
}

/**
 * @brief Calculates the milliseconds elapsed since start() was called.
 * 
 * @return Elapsed milliseconds.
 */
unsigned long RTCManager::elapsMillis() {
  return millis() - startMillis;
}
