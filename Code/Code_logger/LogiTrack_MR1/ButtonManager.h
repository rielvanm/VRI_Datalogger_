#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include "DisplayManager.h"

enum ButtonAction {
  NONE,
  START,
  SAVE,
  DELETE_ACTION,
  STOP
};

class ButtonManager {
  public:
  ButtonManager();
  void begin();
  ButtonAction readButtons();
  //DisplayState getState() const;

  private:
  const int buttonPins[4] = {6, 7, 8, 9};
  unsigned long lastDebounceTime[4];
  bool lastButtonState[4];
  bool buttonState[4];
  static const unsigned long debounceDelay = 20;

};

#endif