#ifndef UTILS_H
#define UTILS_H

#include <Print.h>
#include <RTClib.h>

inline void printDigits(Print& output, int digits) {
  if (digits <10) output.print("0");
  output.print(digits);
}

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