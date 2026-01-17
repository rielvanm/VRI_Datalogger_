#include "SensorTrigger.h"
#include "TriggerBuffer.h"

volatile int SensorTrigger::triggerCount = 0;
volatile uint32_t SensorTrigger::lastTriggerUs = 0;
TriggerBuffer* SensorTrigger::triggerBufferPtr = nullptr;

volatile bool SensorTrigger::armed = false;
uint8_t SensorTrigger::sensorPin = 255;

extern volatile uint32_t interruptCounter;

void SensorTrigger::setTriggerBuffer(TriggerBuffer* buf) {
  triggerBufferPtr = buf;
}

void SensorTrigger::begin(uint8_t pin) {
  sensorPin = pin;
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, FALLING);
  armed = true;
}

void SensorTrigger::arm() {
  if (sensorPin == 255) return;
  lastTriggerUs = (uint32_t)micros(); // startpunt
  attachInterrupt(digitalPinToInterrupt(sensorPin), handleInterrupt, FALLING);
  armed = true;
}

void SensorTrigger::disarm() {
  if (sensorPin == 255) return;
  detachInterrupt(digitalPinToInterrupt(sensorPin));
  armed = false;
  lastTriggerUs = 0;
}

void IRAM_ATTR SensorTrigger::handleInterrupt() {
  if (!armed) return;

  uint32_t nowUs = (uint32_t)micros();

  // eerste trigger na arm(): alleen initialiseren, niet loggen
  if (lastTriggerUs == 0) {
    lastTriggerUs = nowUs;
    return;
  }

  uint32_t deltaUs = nowUs - lastTriggerUs; // wrap-safe

  if (deltaUs > 200000) { // debounce 20ms
    triggerCount++;
    interruptCounter++;
    lastTriggerUs = nowUs;

    if (triggerBufferPtr) {

      triggerBufferPtr->addFromISR(deltaUs, nowUs);
    }
  }
} 

bool SensorTrigger::wasTriggered() {
  if (triggerCount > 0) {
    triggerCount--;
    return true;
  }
  return false;
}
