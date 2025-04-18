#ifndef RTC_H                             /// include guard 
#define RTC_H                             /// define rtc

#include <RTClib.h>                       /// Adafruit library

class RTCManager {                        /// Class RTCManager  
  public:
  void begin();                           /// Object begin starts in setup if no RTC is init, then stops    
  void start();                           /// Object sets time on 0, en sets running == true
  void stopAndReset();                    /// Object stops timer en reset time to 00:00:00 and running == false
  DateTime now();                         /// returns the realtime if false stoptime, true returns real time   
  unsigned long elapsMillis();            /// Returns milliseconds after start
  void setDateTime(int year, int month, int day, int hour, int minute);

  private:
  RTC_DS3231 rtc;                         /// Object of adafruit DS3231 class
  unsigned long startMillis = 0;          /// memorys time in ms (sistik)      
  bool running = false;                   /// essential to know if 
  DateTime startTime;                     /// 
};

#endif
