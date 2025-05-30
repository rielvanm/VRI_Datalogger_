#include "TriggerBuffer.h"
#include <Arduino.h>
#include <vector>
#include "SDManager.h"

extern SDManager sd;  // External instance of SDManager used for writing to SD card


void TriggerBuffer::addFromISR(DateTime time, unsigned long ms) {
  if (isrIndex < MAX_ISR_BUFFER) {
    isrBuffer[isrIndex].time = time;
    isrBuffer[isrIndex].ms = ms;
    isrIndex++;
    pending = true;  // Signal that data is ready to be transferred
  }
}

/**
 * @brief Transfers buffered data from the ISR-safe buffer to the main vector buffer.
 * 
 * This function disables interrupts briefly to safely copy data.
 * The temporary buffer avoids dynamic allocation during the noInterrupts() phase.
 */
void TriggerBuffer::transferPending() {
  if (!pending) return;

  noInterrupts();  // Prevent ISR while copying
  int count = isrIndex;
  isrIndex = 0;
  pending = false;

  TriggerMoment temp[count];  // Temporary local copy
  for (int i = 0; i < count; i++) {
    // Copy using unix timestamp to avoid issues with internal pointers
    temp[i].time = DateTime(static_cast<uint32_t>(isrBuffer[i].time.unixtime()));
    temp[i].ms = isrBuffer[i].ms;
  }
  interrupts();  // Re-enable interrupts

  // Append to main buffer
  for (int i = 0; i < count; i++) {
    mainBuffer.push_back(temp[i]);
  }
}

/**
 * @brief Checks whether there are any unprocessed trigger moments.
 * 
 * @return true if there is at least one event in the main buffer.
 */
bool TriggerBuffer::hasPending() const {
  return !mainBuffer.empty();
}

/**
 * @brief Processes the oldest pending event by writing it to the SD card.
 * 
 * Converts the time to HH:MM:SS.mmm format and writes it to "metingen.csv".
 * Removes the event from the buffer after processing.
 * 
 * @param sd Reference to the SDManager used to write to the SD card.
 */
void TriggerBuffer::processNext(SDManager& sd) {
  if (!hasPending()) return;

  // Get and remove the oldest event
  TriggerMoment evt = mainBuffer.front();
  mainBuffer.erase(mainBuffer.begin());

  // Format timestamp as "HH:MM:SS.mmm"
  char tijd[16];
  snprintf(tijd, sizeof(tijd), "%02d:%02d:%02d.%03lu",
           evt.time.hour(), evt.time.minute(), evt.time.second(),
           evt.ms);

  // Write to CSV file on SD card
  sd.writeLine("metingen.csv", tijd);
}
