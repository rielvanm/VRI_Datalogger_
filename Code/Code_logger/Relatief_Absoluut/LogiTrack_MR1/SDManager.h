#ifndef SD_MANAGER
#define SD_MANAGER

#include <Arduino.h>

/**
 * @class SDManager
 * @brief Handles basic SD card operations such as initialization, writing, and file existence checks.
 *
 * This class simplifies interaction with an SD card using the Arduino SD library.
 * It is intended for use with a CS (Chip Select) pin provided at construction.
 */
class SDManager {
public:
  /**
   * @brief Constructor
   * @param csPin The chip select (CS) pin used to communicate with the SD card.
   */
  SDManager(int csPin);

  /**
   * @brief Initializes communication with the SD card.
   * Should be called in setup().
   * 
   * @return true if initialization was successful, false otherwise.
   */
  bool begin();

  /**
   * @brief Appends a line of text to a file on the SD card.
   * Creates the file if it doesn't exist.
   *
   * @param filename The name of the file (e.g. "/log.txt").
   * @param data     The string to append to the file.
   * @return true if the write operation was successful, false otherwise.
   */
  bool writeLine(const String& filename, const String& data);

  /**
   * @brief Checks whether a specific file exists on the SD card.
   * 
   * @param filename The file to check (e.g. "/config.txt").
   * @return true if the file exists, false otherwise.
   */
  bool fileExists(const String& filename);

private:
  int _csPin;  ///< Chip Select pin number used for SD communication
};

#endif
