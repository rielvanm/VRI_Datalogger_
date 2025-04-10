#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

class RTCManager {
  public:
  void begin();
  void start();
  void stopAndReset();
  DateTime now();
  unsigned long elapsMillis();

  private:
  RTC_DS3231 rtc;
  unsigned long startMillis = 0;
  bool running = false;
  DateTime startTime;
};

#endif
