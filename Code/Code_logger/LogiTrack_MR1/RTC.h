#ifndef RTC_H                         // Include guard to prevent multiple inclusion
#define RTC_H

#include <RTClib.h>                   // Adafruit RTClib library for DS3231 and compatible RTCs

/**
 * @class RTCManager
 * @brief Manages timekeeping using a DS3231 RTC and internal timing logic.
 *
 * This class wraps around an RTC_DS3231 instance and adds functionality
 * such as start/stop timers, elapsed time measurement, and manual time setting.
 */
class RTCManager {
public:
  /**
   * @brief Initializes the RTC module.
   * Should be called in setup(). If no valid RTC is detected, the program will stop.
   */
  void begin();

  /**
   * @brief Starts the timer logic.
   * Stores the current time and sets internal `running` flag to true.
   */
  void start();

  /**
   * @brief Stops the timer and resets the time to 00:00:00.
   * Also clears the `running` flag.
   */
  void stopAndReset();

  /**
   * @brief Returns the current DateTime.
   * If the timer is running, returns real-time from the RTC.
   * If stopped, returns the saved start time.
   */
  DateTime now();

  /**
   * @brief Returns the number of milliseconds that have passed since `start()` was called.
   * If not running, this value will not change.
   */
  unsigned long elapsMillis();

  /**
   * @brief Manually sets the RTC to a specific date and time.
   * @param year   Full year (e.g. 2025)
   * @param month  Month (1–12)
   * @param day    Day (1–31)
   * @param hour   Hour (0–23)
   * @param minute Minute (0–59)
   */
  void setTime(int year, int month, int day, int hour, int minute);

private:
  RTC_DS3231 rtc;                 ///< Instance of Adafruit DS3231 RTC
  unsigned long startMillis = 0;  ///< Time in milliseconds when `start()` was called
  bool running = false;           ///< Whether the timer is actively running
  DateTime startTime;             ///< Stores the time when `start()` was called
};

#endif
