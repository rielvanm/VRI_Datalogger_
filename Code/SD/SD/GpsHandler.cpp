#include "GpsHandler.h"
#include <HardwareSerial.h>

HardwareSerial GPS(1);  // Gebruik hardware serial 1

GpsHandler::GpsHandler(int rx, int tx, int tz) {
  rxPin = rx;
  txPin = tx;
  timeZoneOffset = tz;
}

void GpsHandler::begin() {
  GPS.begin(9600, SERIAL_8N1, rxPin, txPin);
}

void GpsHandler::update() {
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
}

void GpsHandler::printData() {
  if (gps.location.isValid()) {
    Serial.print(F("- latitude: "));
    Serial.println(gps.location.lat(), 6);

    Serial.print(F("- longitude: "));
    Serial.println(gps.location.lng(), 6);

    Serial.print(F("- altitude: "));
    if (gps.altitude.isValid())
      Serial.println(gps.altitude.meters());
    else
      Serial.println(F("INVALID"));
  } else {
    Serial.println(F("- location: INVALID"));
  }

  Serial.print(F("- speed: "));
  if (gps.speed.isValid()) {
    Serial.print(gps.speed.kmph());
    Serial.println(F(" km/h"));
  } else {
    Serial.println(F("INVALID"));
  }

  Serial.print(F("- GPS lokale tijd: "));
  if (gps.date.isValid() && gps.time.isValid()) {
    int correctedHour = gps.time.hour() + timeZoneOffset;
    if (correctedHour >= 24) correctedHour -= 24;

    Serial.print(gps.date.year());
    Serial.print(F("-"));
    printDigits(gps.date.month());
    Serial.print(F("-"));
    printDigits(gps.date.day());
    Serial.print(F(" "));
    printDigits(correctedHour);
    Serial.print(F(":"));
    printDigits(gps.time.minute());
    Serial.print(F(":"));
    printDigits(gps.time.second());
    Serial.print(F("."));
    Serial.println(gps.time.centisecond());
  } else {
    Serial.println(F("INVALID"));
  }

  Serial.println();
}

void GpsHandler::printDigits(int digits) {
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
