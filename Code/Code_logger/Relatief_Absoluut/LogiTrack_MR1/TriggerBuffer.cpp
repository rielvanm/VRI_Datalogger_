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

void TriggerBuffer::processNext(SDManager& sd, TinyGPSPlus* gps) {
  if (!hasPending()) return;

  TriggerMoment evt = mainBuffer.front();
  mainBuffer.erase(mainBuffer.begin());

  // Timer in seconden — meest belangrijke kolom, altijd aanwezig
  double delta_s = evt.delta_us / 1000000.0;
  char deltaStr[12];
  snprintf(deltaStr, sizeof(deltaStr), "%.3f", delta_s);
  for (char* p = deltaStr; *p; p++) {
    if (*p == '.') *p = ',';
  }

  // UTC tijd van detectie — tussen aanhalingstekens zodat Excel de dubbele punten
  // niet als extra kolomscheider interpreteert
  char utcStr[12] = "";
  if (gps != nullptr && gps->time.isValid()) {
    snprintf(utcStr, sizeof(utcStr), "\"%02d:%02d:%02d\"",
             gps->time.hour(), gps->time.minute(), gps->time.second());
  }

  // Lat/lng met 8 decimalen en komma als decimaalscheider
  // Buffer: sign(1) + digits(3) + comma(1) + decimals(8) + null = 14 bytes
  char latStr[14] = "", lngStr[14] = "";
  if (gps != nullptr && gps->location.isValid()) {
    dtostrf(gps->location.lat(), 1, 8, latStr);
    dtostrf(gps->location.lng(), 1, 8, lngStr);
    for (char* p = latStr; *p; p++) if (*p == '.') *p = ',';
    for (char* p = lngStr; *p; p++) if (*p == '.') *p = ',';
  }

  // Snelheid in km/h met komma als decimaalscheider
  char spdStr[10] = "";
  if (gps != nullptr && gps->speed.isValid()) {
    dtostrf(gps->speed.kmph(), 1, 1, spdStr);
    for (char* p = spdStr; *p; p++) if (*p == '.') *p = ',';
  }

  char line[80];
  snprintf(line, sizeof(line), "%s;%s;%s;%s;%s",
           utcStr, deltaStr, latStr, lngStr, spdStr);

  sd.writeLine("metingen.csv", line);
}


