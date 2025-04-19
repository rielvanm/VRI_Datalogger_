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
#define PIN_KLOK 7
#define PIN_RETURN 8 

bool metingGestart = false;
bool metingGestopt = false;
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
  displayManager.begin();                           // Start OLED
  displayManager.showIntro(arduino_icon);           // Show INTRO
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
  displayManager.update(gpsHandler.getGps(), timeZoneOffset, rtcManager.now());       // Update display with new gps-data incl 1 hour summertime

if (sensorTrigger.wasTriggered()) {
  DateTime now = rtcManager.now();
  unsigned long ms = rtcManager.elapsMillis() % 1000;
  triggerBuffer.addFromISR(now, ms); // Veilig
}

triggerBuffer.transferPending(); // veilig kopiëren
if (triggerBuffer.hasPending()) {
  triggerBuffer.processNext(sd);
}
/*
bool startKnopIngedrukt = digitalRead(PIN_START) == LOW;
bool stopKnopIngedrukt  = digitalRead(PIN_STOP)  == LOW;
//bool KlokKnopIngedrukt = digitalRead(PIN_KLOK) == LOW;
//bool ReturnKnopIngedrukt  = digitalRead(PIN_RETURN)  == LOW;

if (startKnopIngedrukt) {
  interruptCounter = 0;  // teller resetten
  displayManager.setState(DisplayManager::DisplayState::Logging);
}

if (stopKnopIngedrukt) {
  displayManager.setState(DisplayManager::DisplayState::Stopped);
}

*/
if (displayManager.getState() == DisplayManager::DisplayState::TimeSet) {
  ButtonAction clkAction = buttons.readSecondButtons();
  if (clkAction != NONE) {
    switch (clkAction) {
      case PLUS:
        displayManager.timeFields[displayManager.selectedField]++;
        break;

      case MIN:
        displayManager.timeFields[displayManager.selectedField]--;
        break;

      case NEXT:
        displayManager.selectedField = (displayManager.selectedField + 1) % 5;
        break;

      case RETURN:
        // Tijd instellen in de RTC
        rtcManager.setTime(
          displayManager.timeFields[2],
          displayManager.timeFields[1],
          displayManager.timeFields[0],
          displayManager.timeFields[3],
          displayManager.timeFields[4]
        );
        displayManager.setState(DisplayManager::DisplayState::Menu);
        break;

      default:
        break;
    }
  }
} else {
  // Alleen als we NIET in TimeSet zitten, andere knoppen verwerken
  ButtonAction action = buttons.readButtons();
  if (action != NONE) {
    switch (action) {
case START:
  if (!metingGestart && displayManager.sdAvailable) {
  displayManager.addUserMessage("Meting gestart");                
  rtcManager.start();
  ritTeller++;
  metingGestart = true;
  metingGestopt = false;
  // CSV-kopregel schrijven

    DateTime now = rtcManager.now();  // tijd vastleggen
    char header[64];
    snprintf(header, sizeof(header), "Rit %d, %d-%02d-%02d, %02d:%02d:%02d",
             ritnummer,
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
    sd.writeLine("metingen.csv", header);
  } else if (!displayManager.sdAvailable) {
    displayManager.addUserMessage("Plaats SD-kaart"); 
  } else {  
    displayManager.addUserMessage("Meting loopt"); 
  }
  break;

      case KLOK:
      displayManager.setState(DisplayManager::DisplayState::TimeSet);                                      // Place holders! No functions for the buttons ye
      display.addUserMessage("Klok menu");
      break;
      
      case SHOW_GPS:
      displayManager.addUserMessage("Nog geen GPS");
      break;

      case STOP:
      if (!metingGestopt) {
      displayManager.addUserMessage("Meting gestopt");
      displayManager.addUserMessage("Detecties: " + String(interruptCounter));
      interruptCounter = 0;
      rtcManager.stopAndReset();
      sd.writeLine("metingen.csv", "");  // lege regel tussen ritten
      metingGestopt = true;
      metingGestart = false;
      } else {
        displayManager.addUserMessage("Was al gestopt");
      }
      break;
      default:
      break;
    }
  }
  delay(50);
}
}