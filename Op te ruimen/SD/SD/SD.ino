#include <TinyGPS++.h>                              // Lyb for translating NMEA data 
#include "DisplayManager.h"                         // Header-file for OLED
#include "GpsHandler.h"
#include "Utils.h"
#include "ButtonManager.h"
#include "SensorTrigger.h"

// GPS-pinnen
#define RX0PIN D0                                   // Res (UART) data from GPS PIN D0
#define TX0PIN D1                                   // Tran (UART) data to GPS PIN D1

SensorTrigger sensorTrigger;
ButtonManager buttons;
DisplayManager display;                             // Create object display in class DisplayManager 
HardwareSerial GPS(1);                              
const int timeZoneOffset = 1;                       // Using local time = UTC + 1
GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset); 

// Externe bitmap (logo)
extern const unsigned char arduino_icon[];          // extern bitmap (array LOGO) 

void setup() {                                      
  Serial.begin(9600);                               // Start USB-serial comm on baudrate 9600 bits/s
  Wire.begin();
  gpsHandler.begin();
  display.begin();                                  // Initialise OLED
  display.showIntro(arduino_icon);                  // Show INTRO
  buttons.begin(); 
  sensorTrigger.begin(D3);
}

void loop() {
  gpsHandler.update(); 
  display.update(gpsHandler.getGps(), timeZoneOffset);       // Update display with new gps-data incl 1 hour summertime
  
  if (sensorTrigger.wasTriggered()) {
    display.showMessage("Sensor triggered tijd wordt stilgezet!");
    // zet hier je logica om RTC-teller te stoppen
  }
  ButtonAction action = buttons.readButtons();
  if (action != NONE) {
    switch (action) {
      case START:
      display.showMessage("Start pressed!");
      break;
      case SAVE:
      display.showMessage("Save pressed!");
      break;
      case DELETE_ACTION:
      display.showMessage("DELET pressed!");
      break;
      case STOP:
      display.showMessage("Stop pressed!");
      break;
      default:
      break;
    }
  }
  delay(100);
}