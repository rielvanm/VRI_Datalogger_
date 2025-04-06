#include "GpsHandler.h"                               /// loading headerfile with class GPShandler
#include <HardwareSerial.h>                           /// loading HardwareSerial class

GpsHandler::GpsHandler(int rx, int tx, int tz) {      /// constructor GpsHandler
  rxPin = rx;
  txPin = tx;
  timeZoneOffset = tz;
}

void GpsHandler::begin() {                            /// Init serial commincation
  GPS.begin(9600, SERIAL_8N1, rxPin, txPin);          /// begin 
}

TinyGPSPlus& GpsHandler::getGps() {
  return gps;
}

void GpsHandler::update() {                           /// reading all char form the GPS en parsing by TinyGPS++
  while (GPS.available()) {                           /// Checking if readinformation is
    gps.encode(GPS.read());                           /// read char and building GPS-information
  }
}

/*
void GpsHandler::printData() {                        /// print all usefull GPS-information to serial monitor
  if (gps.location.isValid()) {                       /// checking if location is valid
    Serial.print(F("- latitude: "));                  /// printing latitude
    Serial.println(gps.location.lat(), 6);            /// printing latitude coordinates 6 decimals 

    Serial.print(F("- longitude: "));                 /// printing longitude
    Serial.println(gps.location.lng(), 6);            /// printing longitude coordinates 6 decimals

    Serial.print(F("- altitude: "));                  /// printing altitude
    if (gps.altitude.isValid())                       /// if valid
      Serial.println(gps.altitude.meters());          /// printing altitude hight  
    else  
      Serial.println(F("INVALID"));                   /// else print invalid
  } else {
    Serial.println(F("- location: INVALID"));
  }

  Serial.print(F("- speed: "));                        /// printing speed    
  if (gps.speed.isValid()) {                           /// If data is valid 
    Serial.print(gps.speed.kmph());                    /// print speed data 
    Serial.println(F(" km/h"));                        /// printing km/h 
  } else {                                             /// else print invalid 
    Serial.println(F("INVALID"));
  }

  Serial.print(F("- GPS lokale tijd: "));                   /// printing GPS time (local)
  if (gps.date.isValid() && gps.time.isValid()) {           /// if date and time is valid
    int correctedHour = gps.time.hour() + timeZoneOffset;   /// correcthour is time + 1 hour (local time)
    if (correctedHour >= 24) correctedHour -= 24;           /// if correcthour +24h is -24 becouse 24 / 25 h doesn't exist

    Serial.print(gps.date.year());                          /// print year                          
    Serial.print(F("-"));                                   /// print -
    printDigits(gps.date.month());                          /// print month
    Serial.print(F("-"));                                   
    printDigits(gps.date.day());                            /// print day    
    Serial.print(F(" "));
    printDigits(correctedHour);                             /// print correct hour
    Serial.print(F(":"));
    printDigits(gps.time.minute());                         /// print minute
    Serial.print(F(":"));
    printDigits(gps.time.second());                         /// print second
    Serial.print(F("."));                                     
    Serial.println(gps.time.centisecond());                 /// print centisecond
  } else {
    Serial.println(F("INVALID"));                           /// else print INVALID
  }

  Serial.println("\n");                                      /// visual space between GPS-blocks 
}

void GpsHandler::printDigits(int digits) {                   ///  
  if (digits < 10)                                           /// if the number is smaller then 10 
    Serial.print('0');                                       /// print first 0 
  Serial.print(digits);                                      /// then print the number by it self 
}
*/