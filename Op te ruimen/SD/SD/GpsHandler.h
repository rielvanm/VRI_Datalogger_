#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>

extern HardwareSerial GPS;          // declaratie van extern object
// extern class GpsHandler gpsHandler; // declaratie van extern object

class GpsHandler {
public:
    GpsHandler(int rxPin, int txPin, int timeZone);
    void begin();
    void update();
    TinyGPSPlus& getGps(); 
    void printData();

private:
    TinyGPSPlus gps;
    int rxPin, txPin, timeZoneOffset;
    void printDigits(int digits);
};

#endif
