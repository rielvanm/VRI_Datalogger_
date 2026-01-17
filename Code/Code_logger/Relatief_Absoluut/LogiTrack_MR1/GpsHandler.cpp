/**
 * @file GpsHandler.cpp
 * @brief Implementation of the GpsHandler class to manage GPS data reception and parsing.
 */

#include "GpsHandler.h"           ///< Include header for GpsHandler class
#include <HardwareSerial.h>       ///< Include HardwareSerial for ESP32 serial port usage


GpsHandler::GpsHandler(int rxPin, int txPin, int timeZone) {
  rxPin = rxPin;
  txPin = txPin;
  timeZoneOffset = timeZone;
}

/**
 * @brief Initializes serial communication with the GPS module.
 *
 * Sets up the UART interface using the provided RX and TX pins
 * and starts communication at 9600 baud.
 */
void GpsHandler::begin() {
  GPS.begin(9600, SERIAL_8N1, rxPin, txPin);
}

/**
 * @brief Returns a reference to the internal TinyGPSPlus instance.
 * @return Reference to TinyGPSPlus object.
 */
TinyGPSPlus& GpsHandler::getGps() {
  return gps;
}

/**
 * @brief Reads available characters from the GPS module and parses them.
 *
 * Continuously reads from the GPS serial buffer and passes the characters
 * to the TinyGPS++ parser.
 */
void GpsHandler::update() {
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
}

/*
// Debugging utility: Print GPS data to Serial monitor
void GpsHandler::printData() {
  if (gps.location.isValid()) {
    Serial.print(F("- Latitude: "));
    Serial.println(gps.location.lat(), 6);

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

  Serial.print(F("- Speed: "));
  if (gps.speed.isValid()) {
    Serial.print(gps.speed.kmph());
    Serial.println(F(" km/h"));
  } else {
    Serial.println(F("INVALID"));
  }

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

  Serial.println("\n");
}

// Helper: Add leading zero to single-digit numbers
void GpsHandler::printDigits(int digits) {
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
*/
