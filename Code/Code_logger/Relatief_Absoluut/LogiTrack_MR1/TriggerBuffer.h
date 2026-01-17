#ifndef TRIGGERBUFFER_H
#define TRIGGERBUFFER_H

#include <vector>
#include <RTClib.h>
#include "SDManager.h"

/**
 * @struct TriggerMoment
 * @brief Represents a single sensor trigger event.
 *
 * Contains a timestamp (RTC-based) and a millisecond offset
 * used for sub-second precision.
 */
struct TriggerMoment {
  uint32_t delta_us;  // tijd sinds vorige detectie
  uint32_t now_us;    // micros() op het moment van detectie
};

/**
 * @class TriggerBuffer
 * @brief Manages buffered trigger events detected via interrupts.
 *
 * Uses a small static array (ISR-safe) to capture triggers from an ISR
 * and transfers them safely into a dynamic buffer (vector) for later processing.
 * This separation ensures ISR safety and avoids heap usage inside the ISR.
 */
class TriggerBuffer {
public:
  /**
   * @brief Called from ISR to store a trigger time (non-blocking).
   * 
   * @param time The current time from RTC at moment of trigger.
   * @param ms   Millisecond value at the time of the event.
   */
  void addFromISR(uint32_t delta_us, uint32_t now_us);
  
  void resetElapsed(); 

  /**
   * @brief Transfers all pending events from the ISR buffer to the main buffer.
   * Should be called regularly from the main loop to flush the ISR buffer.
   */
  void transferPending(); // Copies from isrBuffer to mainBuffer

  /**
   * @brief Processes and removes the oldest event from the buffer.
   * Writes the event as a CSV line using SDManager.
   * 
   * @param sd Reference to SDManager for file writing.
   */
  void processNext(SDManager& sd);

  /**
   * @brief Checks whether there are events waiting to be processed.
   * 
   * @return true if the main buffer is not empty.
   */
  bool hasPending() const;

private:
  static const int MAX_ISR_BUFFER = 10;  ///< Size of the temporary ISR buffer

  // Buffer for ISR context (static allocation)
  TriggerMoment isrBuffer[MAX_ISR_BUFFER];
  volatile int isrIndex = 0;            ///< Index into isrBuffer
  volatile bool pending = false;        ///< Flag indicating new data is ready

  // Main application buffer (dynamic, safe to use outside ISR)
  std::vector<TriggerMoment> mainBuffer;
};

#endif
