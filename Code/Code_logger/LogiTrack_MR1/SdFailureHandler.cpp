#include "SdFailureHandler.h"
#include "DisplayManager.h"
#include "RTC.h"

extern DisplayManager displayManager;
extern RTCManager rtcManager;
extern bool metingGestart;
extern bool metingGestopt;
extern volatile uint32_t interruptCounter;

void handleSdFailureDuringLogging() {
  if (metingGestart && (!displayManager.isSdAvailable() || !displayManager.isSdWritable())) {
    displayManager.addUserMessage("SD verwijderd!");
    displayManager.addUserMessage("Afgebroken!");
    rtcManager.stopAndReset();
    interruptCounter = 0;
    metingGestart = false;
    metingGestopt = true;
  }
}
