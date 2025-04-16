#ifndef TRIGGERBUFFER_H
#define TRIGGERBUFFER_H

#include <RTClib.h>
#include "SDManager.h"

struct TriggerMoment {
  DateTime time;
  unsigned long ms;
};

class TriggerBuffer {
public:
  void add(const DateTime& time, unsigned long ms);
  void processNext(SDManager& sd);
  bool hasPending() const;

private:
  static const int MAX_TRIGGERS = 10;
  TriggerMoment buffer[MAX_TRIGGERS];
  int triggerIndex = 0;
};

#endif
