#include "SensorTrigger.h"

volatile bool SensorTrigger::triggered = false;

void SensorTrigger::begin(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, FALLING);
}

void IRAM_ATTR SensorTrigger::handleInterrupt() {
  triggered = true;
}

bool SensorTrigger::wasTriggered() {
  if (triggered) {
    triggered = false;
    return true;
  }
  return false;
}
