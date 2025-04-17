#include "TriggerBuffer.h"
#include <Arduino.h>
#include <vector>

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
  if (mainBuffer.empty()) return;

  TriggerMoment m = mainBuffer.front();
  mainBuffer.erase(mainBuffer.begin());

  String ts;
  ts.reserve(20);
  //ts += (m.time.hour()   < 10 ? "0" : "") + String(m.time.hour()) + ":";
  ts += (m.time.minute() < 10 ? "0" : "") + String(m.time.minute()) + ":";
  ts += (m.time.second() < 10 ? "0" : "") + String(m.time.second()) + ".";
  if (m.ms < 10) ts += "00";
  else if (m.ms < 100) ts += "0";
  ts += String(m.ms);

  sd.writeLine("trigger_log.txt", ts);
  Serial.println("Buffered trigger opgeslagen: " + ts);
}
