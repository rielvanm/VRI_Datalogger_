#include "ButtonManager.h"

ButtonManager::ButtonManager() {
  for (int i = 0; i < 4; i++) {
    lastDebounceTime[i] = 0;
    lastButtonState[i] = HIGH;
    buttonState[i]; HIGH;
  }
}

void ButtonManager::begin() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

ButtonAction ButtonManager::readButtons() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        if (buttonState[i] == LOW) {
          switch (i) {
            case 0: return START;
            case 1: return SAVE;
            case 2: return DELETE_ACTION;
            case 3: return STOP;
          }
        }
      }
    }
    lastButtonState[i] = reading;
  }
  return NONE;
}