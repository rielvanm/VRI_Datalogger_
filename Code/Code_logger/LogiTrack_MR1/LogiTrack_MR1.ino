#include <TinyGPS++.h>                              // Lyb for translating NMEA data 
#include "DisplayManager.h"                         // Header-file for OLED
#include "GpsHandler.h"                             // GPS-module
#include "Utils.h"                                  // Helpfunction (printing digits)
#include "ButtonManager.h"                          // Buttonpress
#include "SensorTrigger.h"                          // React on trigger interrupt sensor
#include "RTC.h"

// GPS-pinnen
#define RX0PIN D0                                   // Res (UART) data from GPS PIN D0
#define TX0PIN D1                                   // Tran (UART) data to GPS PIN D1

RTCManager rtcManager;
SensorTrigger sensorTrigger;                        // Object react on interrupt
ButtonManager buttons;                              // Object reading buttons
DisplayManager display;                             // Create object display in class DisplayManager 
HardwareSerial GPS(1);                              // Serial 1
const int timeZoneOffset = 1;                       // Using local time = UTC + 1
GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset); // GPS-handler pins and timezone

// Externe bitmap (logo)
extern const unsigned char arduino_icon[];          // extern bitmap (array LOGO) 

void setup() {                                      
  rtcManager.begin();
  Serial.begin(9600);                               // Start USB-serial comm on baudrate 9600 bits/s
  Wire.begin();                                     // Start I2C-bus for OLED and RTC
  gpsHandler.begin();                               // Start GPS-module
  display.begin();                                  // Start OLED
  display.showIntro(arduino_icon);                  // Show INTRO
  buttons.begin();                                  // Init buttons
  sensorTrigger.begin(D3);                          // Init interrupt sensor to D3
 /*
  Serial.begin(9600);
  delay(1000); // wait til ser monitor starts

  // Size sketch (code)
  Serial.print("Sketch size: ");
  Serial.println(ESP.getSketchSize());

  // Free space for new sketch
  Serial.print("Free sketch space: ");
  Serial.println(ESP.getFreeSketchSpace());

  // Total free heap (RAM)
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
*/
}

void loop() {
  gpsHandler.update();                                       // Read new GPS data
  display.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());       // Update display with new gps-data incl 1 hour summertime
  
  if (sensorTrigger.wasTriggered()) {                         // If interrupt was set               
    display.showMessage("Sensor triggered time is stopped!"); // Show message on OLED
    // here comes the code to stop the RTC-timer
  }
  ButtonAction action = buttons.readButtons();                
  if (action != NONE) {
    switch (action) {
      case START:
      display.showMessage("Start pressed!");                  // Place holders! No functions for the buttons yet
      rtcManager.start();
      break;
      case SAVE:
      display.showMessage("Save pressed!");
      break;
      case DELETE_ACTION:
      display.showMessage("Delete pressed!");
      break;
      case STOP:
      display.showMessage("Stop pressed!");
      rtcManager.stopAndReset();
      break;
      default:
      break;
    }
  }
  delay(100);
}