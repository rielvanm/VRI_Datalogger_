#include <TinyGPS++.h>
#include "DisplayManager.h"

// GPS-pinnen
#define RX0PIN D0
#define TX0PIN D1

TinyGPSPlus gps;
DisplayManager display;
HardwareSerial GPS(1);
const int timeZoneOffset = 1;

// Externe bitmap (logo)
extern const unsigned char arduino_icon[];

void setup() {
  Serial.begin(9600);
  GPS.begin(9600, SERIAL_8N1, RX0PIN, TX0PIN);
  display.begin();
  display.showIntro(arduino_icon);
}

void loop() {
  while (GPS.available()) {
    gps.encode(GPS.read());
  }
  display.updateDisplay(gps, timeZoneOffset);
  delay(500);
}
