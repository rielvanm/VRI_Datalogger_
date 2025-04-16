#include "SensorTrigger.h"

volatile int SensorTrigger::triggerCount = 0;

void SensorTrigger::begin(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, FALLING);
}

void IRAM_ATTR SensorTrigger::handleInterrupt() {
  triggerCount++;
}

bool SensorTrigger::wasTriggered() {
  if (triggerCount > 0) {
    triggerCount--;
    return true;
  }
  return false;
}
