#ifndef SENSORTRIGGER_H
#define SENSORTRIGGER_H

#include <Arduino.h>

class SensorTrigger {
public:
  void begin(uint8_t pin);
  bool wasTriggered();  // LET OP: correcte spelling

private:
  static volatile bool triggered;
  static void IRAM_ATTR handleInterrupt();
};

#endif
