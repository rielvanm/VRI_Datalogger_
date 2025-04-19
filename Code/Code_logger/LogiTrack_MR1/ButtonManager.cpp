#include "ButtonManager.h"

ButtonManager::ButtonManager() {      // Constructor
  for (int i = 0; i < 4; i++) {       // Debouncing       
    lastDebounceTime[i] = 0;          // Last
    lastButtonState[i] = HIGH;
    buttonState[i] = HIGH;
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
            case 1: return KLOK;
            case 2: return SHOW_GPS;
            case 3: return STOP;
          }
        }
      }
    }
    lastButtonState[i] = reading;
  }
  return NONE;
}

ButtonAction ButtonManager::readSecondButtons() {
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
            case 0: return PLUS;
            case 1: return NEXT;
            case 2: return RETURN;
            case 3: return MIN;
          }
        }
      }
    }
    lastButtonState[i] = reading;
  }
  return NONE;
}