#include "SDManager.h"
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 4

SDManager::SDManager(int csPin) : _csPin(csPin) {}

bool SDManager::begin() {
  //Serial.println("Initialiseren SPI-bus...");
  SPI.begin(13, 12, 11);
  //Serial.println("SPI-bus actief. Probeer SD.begin()...");
  bool ok = SD.begin(_csPin);
  //if (!ok) Serial.println("SD.begin() faalt.");
  return ok;
}

bool SDManager::writeLine(const String& filename, const String& data) {
  File file = SD.open(filename, FILE_APPEND);
  if (file) {
    file.println(data);
    file.close();
    return true;
  } else {
    Serial.println("Fout bij openen bestand: " + filename);
    return false;
  }
}

bool SDManager::fileExists(const String& filename) {
  return SD.exists("/" + filename);
}