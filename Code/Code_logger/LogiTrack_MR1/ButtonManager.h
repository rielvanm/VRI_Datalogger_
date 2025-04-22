#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "DisplayManager.h"

// Enumeration defining possible button actions
enum ButtonAction {
  NONE,      // No action
  START,     // Start logging
  KLOK,      // Enter clock/time setting
  SHOW_GPS,  // Show GPS information
  STOP,      // Stop logging
  PLUS,      // Increment value (used in time setting)
  MIN,       // Decrement value (used in time setting)
  NEXT,      // Move to next field (used in time setting)
  RETURN     // Confirm or return from current menu
};

// Class for managing button inputs with debouncing logic
class ButtonManager {
  public:
    ButtonManager();                      // Constructor initializes button states
    void begin();                         // Sets up button pins as inputs
    ButtonAction readButtons();           // Reads primary button actions
    ButtonAction readSecondButtons();     // Reads secondary button actions (e.g., setting modes)

  private:
    const int buttonPins[4] = {6, 7, 8, 9};           // Pins connected to buttons
    unsigned long lastDebounceTime[4];                // Last debounce timestamps
    bool lastButtonState[4];                          // Previous button states
    bool buttonState[4];                              // Stable button states after debouncing
    static const unsigned long debounceDelay = 20;    // Debounce delay in milliseconds
    unsigned long lastInterruptTimeDisplay = 0;       // Timestamp for managing display interrupts
};

#endif
