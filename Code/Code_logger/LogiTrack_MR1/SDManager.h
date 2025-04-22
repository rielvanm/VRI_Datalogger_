#ifndef SD_MANAGER
#define SD_MANAGER

#include <Arduino.h>

class SDManager {
  public:
    SDManager(int csPin);
    bool begin();
    bool writeLine(const String& filename, const String& data);  // declaratie
    bool fileExists(const String& filename);

  private:
    int _csPin;
};

#endif
