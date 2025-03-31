#include "DisplayManager.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

DisplayManager::DisplayManager()
: oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1) {}

void DisplayManager::begin() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Failed to initialize SSD1306 OLED"));
    while (1);
  }
  oled.clearDisplay();
  oled.display();
}

void DisplayManager::showIntro(const unsigned char* logo) {
  for (int x = 128; x > -80; x--) {
    oled.clearDisplay();
    oled.drawBitmap(0, 0, logo, 128, 64, WHITE);
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(x, 55);
    oled.println("LogiTrack MR1");
    oled.display();
    delay(50);
  }
  delay(1500);
}

void DisplayManager::updateDisplay(const TinyGPSPlus& gps, int timeZoneOffset) {
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);

  if (gps.location.isValid()) {
    oled.print("Lat: ");
    oled.println(gps.location.lat(), 6);
    oled.print("Lng: ");
    oled.println(gps.location.lng(), 6);
  } else {
    oled.println("Locatie: INVALID");
  }

  if (gps.date.isValid() && gps.time.isValid()) {
    int hour = gps.time.hour() + timeZoneOffset;
    if (hour >= 24) hour -= 24;

    oled.print("Datum: ");
    oled.print(gps.date.day());
    oled.print("-");
    oled.print(gps.date.month());
    oled.print("-");
    oled.println(gps.date.year());

    oled.print("Tijd: ");
    printDigits(hour);
    oled.print(":");
    printDigits(gps.time.minute());
    oled.print(":");
    printDigits(gps.time.second());
    oled.print(".");
    oled.println(gps.time.centisecond());
  } else {
    oled.println("Tijd: INVALID");
  }

  oled.display();
}

void DisplayManager::printDigits(int digits) {
  if (digits < 10) oled.print("0");
  oled.print(digits);
}
