#include "SDManager.h"
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 4  // Default chip select pin (can be overridden via constructor)

// Constructor: store the chip select pin
SDManager::SDManager(int csPin) : _csPin(csPin) {}

/**
 * @brief Initializes SPI and attempts to initialize the SD card.
 * 
 * Uses explicit SPI pin mapping: SCK=13, MISO=12, MOSI=11 (for platforms like ESP32/ESP8266).
 * You can modify these if using different hardware.
 * 
 * @return true if SD card initialization was successful, false otherwise.
 */
bool SDManager::begin() {
  // Initialize SPI with specified pins
  SPI.begin(13, 12, 11);

  // Attempt to initialize SD card with specified CS pin
  bool ok = SD.begin(_csPin);
  return ok;
}

/**
 * @brief Appends a line of text to a file on the SD card.
 * 
 * Automatically prepends '/' to the filename and opens the file in append mode.
 * Closes the file after writing to avoid corruption.
 */
bool SDManager::writeLine(const String& filename, const String& data) {
  String fullPath = "/" + filename;

  Serial.print("Attempting to open file: ");
  Serial.println(fullPath);

  File file = SD.open(fullPath.c_str(), FILE_APPEND);
  if (file) {
    file.println(data);
    file.close();
    Serial.println("Data successfully written.");
    return true;
  } else {
    Serial.println("Failed to open file: " + fullPath);
    return false;
  }
}

/**
 * @brief Checks if a file exists on the SD card.
 * 
 * @param filename Name of the file (without leading '/')
 * @return true if the file exists, false otherwise
 */
bool SDManager::fileExists(const String& filename) {
  return SD.exists("/" + filename);
}
