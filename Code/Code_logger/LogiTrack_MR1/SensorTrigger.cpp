/***
* @file SensorTrigger.cpp
* @brief Implematation of the SensorTrigger class
* 
* This file contains the implemantation of the SensorTrigger class,
* wich manages detection and counting of sensor-triggered interrupts 
*/

#include "SensorTrigger.h"

// Static variable to track the number of valid sensor triggers
volatile int SensorTrigger::triggerCount = 0;

// External global interrupt counter (used elsewhere in the project)
extern volatile uint32_t interruptCounter;

// Static timestamp for debounce handling
unsigned long SensorTrigger::lastInterruptTime = 0;

/**
 * @brief Initialize the sensor pin and attach an interrupt to it.
 * @param pin The GPIO pin number connected to the sensor.
 */
void SensorTrigger::begin(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);  // Configure pin with internal pull-up resistor
  attachInterrupt(digitalPinToInterrupt(pin), handleInterrupt, FALLING);  // Trigger on falling edge
}

/**
 * @brief Interrupt service routine (ISR) that runs on sensor trigger.
 * This is marked with IRAM_ATTR to place it in IRAM for ESP32 compatibility.
 */
void IRAM_ATTR SensorTrigger::handleInterrupt() {
  unsigned long now = millis();

  // Basic software debounce: only accept triggers spaced by > 50ms
  if (now - lastInterruptTime > 20) {
    triggerCount++;         // Local counter (for this class)
    interruptCounter++;     // Global counter (for logging or display)
    lastInterruptTime = now;
  }
}

/**
 * @brief Check if the sensor was triggered since the last call.
 * This method consumes one trigger from the buffer if available.
 * @return true if there was a trigger, false otherwise.
 */
bool SensorTrigger::wasTriggered() {
  if (triggerCount > 0) {
    triggerCount--;
    return true;
  }
  return false;
}
