#include "GpsHandler.h"           // Include header for GpsHandler class
#include <HardwareSerial.h>       // Include HardwareSerial for ESP32 serial port usage

// Constructor: saves the RX, TX pin numbers and time zone offset
GpsHandler::GpsHandler(int rx, int tx, int tz) {
  rxPin = rx;
  txPin = tx;
  timeZoneOffset = tz;
}

// Initialize serial communication with the GPS module
void GpsHandler::begin() {
  GPS.begin(9600, SERIAL_8N1, rxPin, txPin);  // Start GPS serial communication at 9600 baud
}

// Return reference to internal TinyGPS++ instance
TinyGPSPlus& GpsHandler::getGps() {
  return gps;
}

// Read all available characters from GPS and feed them to the TinyGPS++ parser
void GpsHandler::update() {
  while (GPS.available()) {
    gps.encode(GPS.read());  // Feed characters into GPS parser
  }
}

/*
// Print useful GPS data to the serial monitor for debugging
void GpsHandler::printData() {
  // Check if location data is valid
  if (gps.location.isValid()) {
    Serial.print(F("- Latitude: "));
    Serial.println(gps.location.lat(), 6);  // 6 decimal precision

    Serial.print(F("- Longitude: "));
    Serial.println(gps.location.lng(), 6);

    Serial.print(F("- Altitude: "));
    if (gps.altitude.isValid())
      Serial.println(gps.altitude.meters());
    else
      Serial.println(F("INVALID"));
  } else {
    Serial.println(F("- Location: INVALID"));
  }

  // Speed in km/h
  Serial.print(F("- Speed: "));
  if (gps.speed.isValid()) {
    Serial.print(gps.speed.kmph());
    Serial.println(F(" km/h"));
  } else {
    Serial.println(F("INVALID"));
  }

  // Date and time output (corrected with timezone offset)
  Serial.print(F("- Local GPS Time: "));
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

  Serial.println("\n"); // Empty line for readability
}

// Print helper: ensures leading zero for values < 10
void GpsHandler::printDigits(int digits) {
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
*/
