/**
 * @file ButtonManager.cpp
 * @brief Implements debounced button handling for primary and secondary actions.
 */

#include "ButtonManager.h"

/**
 * @brief Constructs the ButtonManager and initializes internal state.
 */
ButtonManager::ButtonManager() {
  for (int i = 0; i < 4; i++) {
    lastDebounceTime[i] = 0;         ///< Timestamp of the last debounce event
    lastButtonState[i] = HIGH;       ///< Last read state of the button
    buttonState[i] = HIGH;           ///< Stable button state after debouncing
  }
}

/**
 * @brief Initializes all button pins as INPUT_PULLUP.
 */
void ButtonManager::begin() {
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

/**
 * @brief Reads button states and returns a primary ButtonAction if pressed.
 *
 * Uses debounce logic to ensure stable input. Mapped to actions:
 * - Index 0: START
 * - Index 1: KLOK
 * - Index 2: SHOW_GPS
 * - Index 3: STOP
 *
 * @return ButtonAction representing the pressed button, or NONE if no action.
 */
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

/**
 * @brief Reads button states and returns a secondary ButtonAction if pressed.
 *
 * Used in time-setting modes. Mapped to actions:
 * - Index 0: PLUS
 * - Index 1: NEXT
 * - Index 2: RETURN
 * - Index 3: MIN
 *
 * @return ButtonAction representing the pressed button, or NONE if no action.
 */
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
