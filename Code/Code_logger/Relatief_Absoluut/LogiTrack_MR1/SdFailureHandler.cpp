#include "SdFailureHandler.h"
#include "DisplayManager.h"
#include "RTC.h"

// External dependencies (shared state and managers)
extern DisplayManager displayManager;
extern RTCManager rtcManager;
extern bool loggingStarted;
extern bool loggingStopped;
extern volatile uint32_t interruptCounter;

/**
 * @brief Handles a failure in SD card availability or writability during active logging.
 * 
 * This function should be called periodically or after write attempts
 * to check if the SD card has become unavailable or unwritable.
 * If so, it stops the logging process, resets internal counters and timers,
 * and provides user feedback via the display.
 */
void handleSdFailureDuringLogging() {
  // Only handle failure if logging is currently active
  if (loggingStarted && (!displayManager.isSdAvailable() || !displayManager.isSdWritable())) {

    // Display error messages to the user
    displayManager.addUserMessage("SD verwijderd!");
    displayManager.addUserMessage("Logging gestopt!");

    // Reset time tracking and counters
    rtcManager.stopAndReset();
    interruptCounter = 0;

    // Update logging state
    loggingStarted = false;
    loggingStopped = true;
  }
}
