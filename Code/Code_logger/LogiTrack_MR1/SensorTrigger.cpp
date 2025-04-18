#include "SensorTrigger.h"

volatile int SensorTrigger::triggerCount = 0;
extern volatile uint32_t interruptCounter;
unsigned long SensorTrigger::lastInterruptTime = 0;

void SensorTrigger::begin(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, FALLING);
}

void IRAM_ATTR SensorTrigger::handleInterrupt() {
  unsigned long now = millis();
  if (now - lastInterruptTime > 50) {  // debounce van 50 ms
    triggerCount++;
    interruptCounter++;
    lastInterruptTime = now;
  }
}

bool SensorTrigger::wasTriggered() {
  if (triggerCount > 0) {
    triggerCount--;
    return true;
  }
  return false;
}
