#ifndef UTILS_H
#define UTILS_H

#include <Print.h>
#include <RTClib.h>

//
// Utils.h
// --------
// Contains utility functions for formatting time and date output,
// especially for use with RTC and OLED/Serial displays.
//

/**
 * @brief Print a two-digit number with leading zero if needed.
 * This is typically used for formatting time values like "09:05".
 *
 * @param output Reference to a Print-compatible object (e.g., Serial or display).
 * @param digits The number to print (0–99).
 */
inline void printDigits(Print& output, int digits) {
  if (digits < 10) output.print("0");
  output.print(digits);
}

/**
 * @brief Print a formatted date and time to Serial in Dutch-style layout.
 * Format: "Datum: yyyy-mm-dd" and "Tijd: hh:mm:ss"
 *
 * @param dt A DateTime object from RTClib (e.g., DS3231 or similar).
 */
inline void printDateTimeToSerial(const DateTime& dt) {
  Serial.print("Datum: ");
  Serial.print(dt.year());
  Serial.print("-");
  if (dt.month() < 10) Serial.print("0");
  Serial.print(dt.month());
  Serial.print("-");
  if (dt.day() < 10) Serial.print("0");
  Serial.println(dt.day());

  Serial.print("Tijd: ");
  if (dt.hour() < 10) Serial.print("0");
  Serial.print(dt.hour());
  Serial.print(":");
  if (dt.minute() < 10) Serial.print("0");
  Serial.print(dt.minute());
  Serial.print(":");
  if (dt.second() < 10) Serial.print("0");
  Serial.println(dt.second());
}

#endif
