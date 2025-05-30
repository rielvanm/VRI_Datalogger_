#include "DisplayManager.h"                                     /// headerfile 
#include "Utils.h"

#define OLED_WIDTH 128                                           /// macro width OLED 128 pix 
#define OLED_HEIGHT 64                                           /// macro hight OLED 64 pix  

DisplayManager::DisplayManager()                                 /// init OLED variable 
: oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}                    /// with hight en width on I2C-bus and no reset pin (-1)

void DisplayManager::begin() {                                    /// init OLED-screen
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {                  /// if init NOT commplete then
    oled.println(F("Failed to initialize SSD1306 OLED"));       /// print fail
    while (1);                                                    /// 
  }
  oled.clearDisplay();                                            /// Clean en erase
  oled.display();                                                 /// 
}

void DisplayManager::showIntro(const unsigned char* logo) {      
  currentState = DisplayState::Intro;
  stateStartTime = millis();

  for (int x = 128; x > -80; x--) {                                /// moving the printed text 
    oled.clearDisplay();                                           /// Clear display 
    oled.drawBitmap(0, 0, logo, 128, 64, WHITE);                   /// (0, 0) left corner, logo = arduino_icon[], 128 x 64 pi, white is collor 
    oled.setTextSize(1);                                           /// text size
    oled.setTextColor(WHITE);                                      /// text collor
    oled.setCursor(x, 55);                                         /// x moving text        
    oled.println("LogiTrack MR1");                                 /// is text 
    oled.display();                                                /// Show on screen 
    delay(50);                                                     /// delay for frame (moving text is smooth) 
  }
  // delay(1500);                                                      
}

void DisplayManager::update(TinyGPSPlus& gps, int timeZoneOffset) {
  switch (currentState) {
    case DisplayState::Intro:
    if (millis() - stateStartTime > 2000) {
      currentState = DisplayState::Menu;
      stateStartTime = millis();
    }
    break;

    case DisplayState::Menu:
    showMenu();
    break;

    case DisplayState::GpsDisplay:
    showGps(gps, timeZoneOffset);
    break;
  }
}

void DisplayManager::showMenu() {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 56);
  oled.println(F("Start"));
  oled.setCursor(36, 56);
  oled.println(F("Save"));
  oled.setCursor(66, 56);
  oled.println(F("Delet"));
  oled.setCursor(102, 56);
  oled.println(F("Stop"));

  oled.setCursor(0, 0);
  oled.print(F("T:21"));
  oled.setCursor(31, 0);
  oled.print(F("km/h:161"));
  oled.setCursor(86, 0);
  oled.print(F("GPS:Ok"));
  
  oled.setCursor(0, 45);
  oled.print(F("Time:00:00:00;000"));

  oled.setCursor(0, 28); 
  oled.println(F("TEST SENSOR: "));
  oled.display();
}

void DisplayManager::showGps(TinyGPSPlus& gps, int timeZoneOffset) {
  updateDisplay(gps, timeZoneOffset);
}

void DisplayManager::showMessage(const char* message) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 10);  // midden van het scherm ongeveer
  oled.println(message);
  oled.display();
  delay(1000);  // Laat de tekst even zichtbaar staan
}

/// erease screen, init text-collor, cursor at begin
void DisplayManager::updateDisplay(TinyGPSPlus& gps, int timeZoneOffset) {
    oled.clearDisplay();              // Wis het scherm (zwart)
    oled.setTextSize(1);             // Tekstgrootte op standaard zetten (klein)
    oled.setTextColor(WHITE);        // Tekstkleur = wit (pixels aan)
    oled.setCursor(0, 0);            // Cursor linksboven zetten


  if (gps.location.isValid()) {
    oled.print(F("Lat: "));
    oled.println(gps.location.lat(), 8);
    oled.print(F("Lng: "));
    oled.println(gps.location.lng(), 8);
  } else {
    oled.println("Location: INVALID");
  }

  if (gps.date.isValid() && gps.time.isValid()) {
    int hour = gps.time.hour() + timeZoneOffset;
    if (hour >= 24) hour -= 24;

    oled.print(F("Date: "));
    printDigits(oled, gps.date.day());
    oled.print(F("-"));
    printDigits(oled,gps.date.month());
    oled.print(F("-"));
    oled.println(gps.date.year());

    oled.print(F("Time(UTC): "));
    printDigits(oled, hour);
    oled.print(F(":"));
    printDigits(oled, gps.time.minute());
    oled.print(F(":"));
    printDigits(oled, gps.time.second());
  } else {
    oled.println("Time: INVALID");
  }

  oled.display();
}
