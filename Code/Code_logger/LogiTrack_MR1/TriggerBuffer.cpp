#include "TriggerBuffer.h"
#include <Arduino.h>
#include <vector>
#include "SDManager.h"

extern SDManager sd;

void TriggerBuffer::addFromISR(DateTime time, unsigned long ms) {
  if (isrIndex < MAX_ISR_BUFFER) {
    isrBuffer[isrIndex].time = time;
    isrBuffer[isrIndex].ms = ms;
    isrIndex++;
    pending = true;
  }
}

void TriggerBuffer::transferPending() {
  if (!pending) return;

  noInterrupts(); // voorkom dat ISR ingrijpt tijdens kopie
  int count = isrIndex;
  isrIndex = 0;
  pending = false;
  TriggerMoment temp[count];
  for (int i = 0; i < count; i++) {
    temp[i].time = DateTime(static_cast<uint32_t>(isrBuffer[i].time.unixtime()));
    temp[i].ms = isrBuffer[i].ms;
  }
  interrupts();

  for (int i = 0; i < count; i++) {
    mainBuffer.push_back(temp[i]);
  }
}

bool TriggerBuffer::hasPending() const {
  return !mainBuffer.empty();
}

void TriggerBuffer::processNext(SDManager& sd) {
  if (!hasPending()) return;

  TriggerMoment evt = mainBuffer.front();
  mainBuffer.erase(mainBuffer.begin());

char tijd[16];
snprintf(tijd, sizeof(tijd), "%02d:%02d:%02d.%03lu",
         evt.time.hour(), evt.time.minute(), evt.time.second(),
         evt.ms);

sd.writeLine("metingen.csv", tijd);
}
