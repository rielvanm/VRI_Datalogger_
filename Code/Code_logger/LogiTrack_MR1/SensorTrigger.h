/**
 * @file SensorTrigger.h
 * @brief Declaration of the SensorTrigger class for hardware interrupt-based sensor input.
 *
 * This class is designed to handle digital sensor triggers using external interrupts on 
 * platforms such as the ESP32 or Arduino. It includes a basic software debounce mechanism 
 * to reduce false triggers caused by signal bouncing.
 */

#ifndef SENSORTRIGGER_H
#define SENSORTRIGGER_H

#include <Arduino.h>

//
// SensorTrigger class
// --------------------
// This class handles hardware interrupt-based detection from a digital sensor.
// It includes basic software debounce and a simple buffer mechanism to check for triggers
// from the main loop without missing interrupts.
//

/**
* @class SensorTrigger
* @brief class for handling sensor input via hardware interrupts.
* 
* This class provides functionality to initialize a sensor pin
* and handle trigger events using interrupts.
*/
class SensorTrigger {
public:
  /**
   * @brief Initialize the sensor input pin and attach an interrupt.
   * @param pin The GPIO pin connected to the sensor. Must support interrupts.
   */
  void begin(uint8_t pin);

  /**
   * @brief Check if the sensor was triggered since the last call.
   * Decrements the internal trigger count.
   * @return true if a new trigger was detected, false otherwise.
   */
  bool wasTriggered();  

private:
  // Static volatile counter for number of valid sensor triggers
  static volatile int triggerCount;

  /**
   * @brief Interrupt service routine (ISR) to be called when sensor is triggered.
   * Handles basic software debouncing.
   */
  static void IRAM_ATTR handleInterrupt();

  // Stores timestamp of last valid trigger to implement debounce
  static unsigned long lastInterruptTime;
};

#endif
