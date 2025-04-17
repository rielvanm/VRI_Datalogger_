#ifndef TRIGGERBUFFER_H
#define TRIGGERBUFFER_H

#include <vector>
#include <RTClib.h>
#include "SDManager.h"


struct TriggerMoment {
  DateTime time;
  unsigned long ms;
};

class TriggerBuffer {
public:
  void addFromISR(DateTime time, unsigned long ms);
  void transferPending(); // kopieert naar vector
  void processNext(SDManager& sd);
  bool hasPending() const;

private:
  static const int MAX_ISR_BUFFER = 10;

  // ISR-buffer
  TriggerMoment isrBuffer[MAX_ISR_BUFFER];
  volatile int isrIndex = 0;
  volatile bool pending = false;

  // Hoofd-buffer (heap, veilig)
  std::vector<TriggerMoment> mainBuffer;
};

#endif
