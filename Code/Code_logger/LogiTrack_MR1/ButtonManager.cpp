// Updated ButtonManager class implementation with useful comments
#include "ButtonManager.h"

// Constructor: initializes button states and debounce variables
ButtonManager::ButtonManager() {
  for (int i = 0; i < 4; i++) {
    lastDebounceTime[i] = 0;         // Timestamp of the last debounce
    lastButtonState[i] = HIGH;       // Last known button state
    buttonState[i] = HIGH;           // Current stable button state
  }
}

// Initializes button pins as inputs with internal pull-up resistors
void ButtonManager::begin() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

// Reads and processes button states for primary button actions with debounce logic
ButtonAction ButtonManager::readButtons() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);

    // Detect button state changes
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    // Debounce check
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;

        // Button pressed action
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
    lastButtonState[i] = reading; // Update last state for next loop iteration
  }
  return NONE;
}

// Reads and processes button states for secondary button actions (e.g., time adjustments)
ButtonAction ButtonManager::readSecondButtons() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);

    // Detect button state changes
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    // Debounce check
    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;

        // Button pressed action
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
    lastButtonState[i] = reading; // Update last state for next loop iteration
  }
  return NONE;
}