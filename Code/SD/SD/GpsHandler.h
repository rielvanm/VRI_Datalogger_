#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <TinyGPS++.h>

class GpsHandler {
public:
    GpsHandler(int rxPin, int txPin, int timeZone);
    void begin();
    void update();
    void printData();

private:
    TinyGPSPlus gps;
    int rxPin, txPin, timeZoneOffset;
    void printDigits(int digits);
};

#endif
