/***
* @file GpsHandler.h
* @brief Declares the GpsHandler class for managing GPS comminucation and parsing.
*/
#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <TinyGPS++.h>
#include <HardwareSerial.h>

// External declaration of the GPS serial object (usually Serial1 on ESP32)
extern HardwareSerial GPS;

/*
// If you need to access a global instance of this class, you can uncomment this line:
extern class GpsHandler gpsHandler;
*/

//
// GpsHandler class
// ----------------
// Encapsulates configuration, reading, and access to GPS data using TinyGPS++
// Provides easy access to GPS values and optional serial output for debugging.
//

/**
* @class GpsHandler
* @brief Handles GPS data using the TinyGPS++ library
* 
* The GpsHandler class manages serial communication with a GPS module,
* parses incoming NMEA sentences using TinyGPS++, and provides access
* to GPS-derived data such as location, date, and time.
*/
class GpsHandler {
public:
    /**
     * @brief Constructor
     * @param rxPin The RX pin to receive GPS data (from GPS TX)
     * @param txPin The TX pin to send data (often unused)
     * @param timeZone Time zone offset in hours (for display correction)
     */
    GpsHandler(int rxPin, int txPin, int timeZone);

    /**
     * @brief Initialize the GPS serial connection
     */
    void begin();

    /**
     * @brief Feed new GPS data from the serial stream to the parser
     */
    void update();

    /**
     * @brief Get a reference to the TinyGPSPlus instance
     * @return Reference to internal gps object
     */
    TinyGPSPlus& getGps(); 

    /**
     * @brief Print GPS location and time info to Serial (for debugging)
     */
    void printData();

private:
    TinyGPSPlus gps;       ///< Parser object for incoming GPS-messages
    int rxPin, txPin;      ///< Hardware serial pin numbers
    int timeZoneOffset;    ///< Offset from UTC in hours for local time display

    /**
     * @brief Print a number with leading zero if needed (helper function)
     */
    void printDigits(int digits);
};

#endif
