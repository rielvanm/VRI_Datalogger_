#ifndef SENSORTRIGGER_H
#define SENSORTRIGGER_H

#include <Arduino.h>

class SensorTrigger {
public:
  void begin(uint8_t pin);
  bool wasTriggered();  

private:
  static volatile int triggerCount;
  static void IRAM_ATTR handleInterrupt();
  static unsigned long lastInterruptTime;

};

#endif
