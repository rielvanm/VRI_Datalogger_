/**
 * @file ButtonManager.h
 * @brief Handles input from buttons using debouncing and interprets them into user actions.
 */

#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "DisplayManager.h"

/**
 * @enum ButtonAction
 * @brief Represents the different possible actions triggered by button inputs.
 */
enum ButtonAction {
  NONE,      ///< No action detected
  START,     ///< Start logging session
  KLOK,      ///< Enter clock/time setting menu
  SHOW_GPS,  ///< Show GPS status/info
  STOP,      ///< Stop logging session
  PLUS,      ///< Increment a value (e.g., date/time)
  MIN,       ///< Decrement a value (e.g., date/time)
  NEXT,      ///< Move to the next field (e.g., in time setting)
  RETURN     ///< Confirm or exit menu
};

/**
 * @class ButtonManager
 * @brief Manages button input using software debouncing.
 *
 * The ButtonManager class reads button states from hardware input pins,
 * applies debouncing logic, and translates button presses into ButtonAction enums.
 */
class ButtonManager {
  public:
    /**
     * @brief Constructs the ButtonManager and initializes button states.
     */
    ButtonManager();

    /**
     * @brief Initializes button input pins with internal pull-up resistors.
     */
    void begin();

    /**
     * @brief Reads button input and returns a primary ButtonAction.
     * @return The detected ButtonAction or NONE.
     */
    ButtonAction readButtons();

    /**
     * @brief Reads button input and returns a secondary ButtonAction (used in setting menus).
     * @return The detected ButtonAction or NONE.
     */
    ButtonAction readSecondButtons();

  private:
    const int buttonPins[4] = {6, 7, 8, 9};                 ///< Pin numbers connected to buttons
    unsigned long lastDebounceTime[4];                      ///< Timestamps of last debounce events
    bool lastButtonState[4];                                ///< Last read states of each button
    bool buttonState[4];                                    ///< Stable states after debounce
    static const unsigned long debounceDelay = 20;          ///< Debounce delay in milliseconds
    unsigned long lastInterruptTimeDisplay = 0;             ///< Timestamp for display update timing
};

#endif // BUTTONMANAGER_H
