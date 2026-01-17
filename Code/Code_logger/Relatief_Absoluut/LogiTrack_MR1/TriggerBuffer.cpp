#include "TriggerBuffer.h"
#include <Arduino.h>
#include <stdio.h>

static uint64_t elapsedUs = 0;

void TriggerBuffer::resetElapsed() {
  elapsedUs = 0;
}

void TriggerBuffer::addFromISR(uint32_t delta_us, uint32_t now_us) {
  if (isrIndex < MAX_ISR_BUFFER) {
    isrBuffer[isrIndex].delta_us = delta_us;
    isrBuffer[isrIndex].now_us   = now_us;
    isrIndex++;
    pending = true;
  }
}

void TriggerBuffer::transferPending() {
  if (!pending) return;

  noInterrupts();
  int count = isrIndex;
  isrIndex = 0;
  pending = false;

  TriggerMoment local[MAX_ISR_BUFFER];
  for (int i = 0; i < count; i++) {
    local[i] = isrBuffer[i]; // kopieer struct
  }
  interrupts();

  for (int i = 0; i < count; i++) {
    mainBuffer.push_back(local[i]);
  }
}

bool TriggerBuffer::hasPending() const {
  return !mainBuffer.empty();
}

void TriggerBuffer::processNext(SDManager& sd) {
  if (!hasPending()) return;

  TriggerMoment evt = mainBuffer.front();
  mainBuffer.erase(mainBuffer.begin());

  double delta_s = evt.delta_us / 1000000.0;

  char line[24];
  snprintf(line, sizeof(line), "%.3f", delta_s);

  // >>> HIER: punt vervangen door komma (voor NL Excel)
  for (char* p = line; *p; p++) {
    if (*p == '.') *p = ',';
  }

  sd.writeLine("metingen.csv", line);
}


