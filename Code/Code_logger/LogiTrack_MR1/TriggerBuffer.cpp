#include "TriggerBuffer.h"
#include <Arduino.h>

void TriggerBuffer::add(const DateTime& time, unsigned long ms) {
  if (triggerIndex < MAX_TRIGGERS) {
    buffer[triggerIndex].time = time;
    buffer[triggerIndex].ms = ms;
    triggerIndex++;
  }
}

void TriggerBuffer::processNext(SDManager& sd) {
  if (triggerIndex == 0) return;

  TriggerMoment& m = buffer[0];
  String ts;
  ts.reserve(20);
  ts += (m.time.hour() < 10 ? "0" : "") + String(m.time.hour()) + ":";
  ts += (m.time.minute() < 10 ? "0" : "") + String(m.time.minute()) + ":";
  ts += (m.time.second() < 10 ? "0" : "") + String(m.time.second()) + ".";
  if (m.ms < 10) ts += "00";
  else if (m.ms < 100) ts += "0";
  ts += String(m.ms);

  sd.writeLine("trigger_log.txt", ts);
  Serial.println("Buffered trigger opgeslagen: " + ts);

  for (int i = 1; i < triggerIndex; i++) {
    buffer[i - 1] = buffer[i];
  }
  triggerIndex--;
}

bool TriggerBuffer::hasPending() const {
  return triggerIndex > 0;
}
