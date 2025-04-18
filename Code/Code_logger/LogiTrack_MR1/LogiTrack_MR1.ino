#include <TinyGPS++.h>                              // Lyb for translating NMEA data 
#include "DisplayManager.h"                         // Header-file for OLED
#include "GpsHandler.h"                             // GPS-module
#include "Utils.h"                                  // Helpfunction (printing digits)
#include "ButtonManager.h"                          // Buttonpress
#include "SensorTrigger.h"                          // React on trigger interrupt sensor
#include "RTC.h"
#include "SDManager.h"
#include "TriggerBuffer.h"

// GPS-pinnen
#define RX0PIN D0                                   // Res (UART) data from GPS PIN D0
#define TX0PIN D1                                   // Tran (UART) data to GPS PIN D1
#define PIN_START 6
#define PIN_STOP  9

int ritnummer = 0;
uint32_t ritTeller = 0; // Telt het aantal volledige metingen
volatile uint32_t interruptCounter = 0;  // externe teller
TriggerBuffer triggerBuffer;
SDManager sd(4);
RTCManager rtcManager;
SensorTrigger sensorTrigger;                        // Object react on interrupt
ButtonManager buttons;                              // Object reading buttons
DisplayManager display;                             // Create object display in class DisplayManager 
DisplayManager displayManager;
HardwareSerial GPS(1);                              // Serial 1
const int timeZoneOffset = 1;                       // Using local time = UTC + 1
GpsHandler gpsHandler(RX0PIN, TX0PIN, timeZoneOffset); // GPS-handler pins and timezone

// Externe bitmap (logo)
extern const unsigned char arduino_icon[];          // extern bitmap (array LOGO) 

void setup() {                                    
  rtcManager.begin();
  Serial.begin(115200);                             // Start USB-serial comm on baudrate 9600 bits/s
  Wire.begin();                                     // Start I2C-bus for OLED and RTC
  gpsHandler.begin();                               // Start GPS-module
  display.begin();                                  // Start OLED
  display.showIntro(arduino_icon);                  // Show INTRO
  buttons.begin();                                  // Init buttons
  sensorTrigger.begin(D3);                          // Init interrupt sensor to D3
  DateTime nu = rtcManager.now();
  printDateTimeToSerial(nu);  
  pinMode(PIN_START, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);

  if (!sd.begin()) {
    Serial.println("SD-kaart initialisatie mislukt.");
  } else {
    Serial.println("SD-kaart succesvol geïnitialiseerd.");
  }
  }
  
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

void loop() {
  gpsHandler.update();                                       // Read new GPS data
  display.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());       // Update display with new gps-data incl 1 hour summertime

if (sensorTrigger.wasTriggered()) {
  DateTime now = rtcManager.now();
  unsigned long ms = rtcManager.elapsMillis() % 1000;
  triggerBuffer.addFromISR(now, ms); // Veilig
}

triggerBuffer.transferPending(); // veilig kopiëren
if (triggerBuffer.hasPending()) {
  triggerBuffer.processNext(sd);
}

bool startKnopIngedrukt = digitalRead(PIN_START) == LOW;
bool stopKnopIngedrukt  = digitalRead(PIN_STOP)  == LOW;

if (startKnopIngedrukt) {
  interruptCounter = 0;  // teller resetten
  displayManager.setState(DisplayManager::DisplayState::Logging);
}

if (stopKnopIngedrukt) {
  displayManager.setState(DisplayManager::DisplayState::Stopped);
}

  ButtonAction action = buttons.readButtons();                
  if (action != NONE) {
    switch (action) {
case START:
  display.addUserMessage("Meting gestart");                
  rtcManager.start();
  ritTeller++;
  // CSV-kopregel schrijven
  {
    DateTime now = rtcManager.now();  // tijd vastleggen
    char header[64];
    snprintf(header, sizeof(header), "Rit %d, %d-%02d-%02d, %02d:%02d:%02d",
             ritnummer,
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    sd.writeLine("metingen.csv", header);
  }

  break;

      case SAVE:                                      // Place holders! No functions for the buttons yet
      display.showMessage("Save pressed!");
      break;
      case DELETE_ACTION:
      display.showMessage("Delete pressed!");
      break;
      case STOP:
      display.addUserMessage("Meting gestopt");
      display.addUserMessage("Detecties: " + String(interruptCounter));
      rtcManager.stopAndReset();
      sd.writeLine("metingen.csv", "");  // lege regel tussen ritten
      break;
      default:
      break;
    }
  }
  delay(50);
}