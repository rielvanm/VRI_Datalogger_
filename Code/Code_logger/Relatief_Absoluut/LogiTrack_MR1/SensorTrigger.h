#ifndef SENSORTRIGGER_H
#define SENSORTRIGGER_H

#include <Arduino.h>

class TriggerBuffer; // forward declaration

class SensorTrigger {
public:
  void begin(uint8_t pin);
  void arm();
  void disarm();

  bool wasTriggered();
  void setTriggerBuffer(TriggerBuffer* buf);
  static void IRAM_ATTR handleInterrupt();

private:
  static volatile int triggerCount;
  static volatile uint32_t lastTriggerUs;
  static TriggerBuffer* triggerBufferPtr;

  static volatile bool armed;
  static uint8_t sensorPin;
};

#endif // SENSORTRIGGER_H
