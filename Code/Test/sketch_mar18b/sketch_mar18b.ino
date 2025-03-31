#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
RTC_DS3231 rtc; // RTC object

void setup() {
    Serial.begin(115200);
    Wire.begin();  // Start I2C-bus voor zowel OLED als RTC
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // OLED initialiseren
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 OLED niet gevonden!"));
        while (1);
    }

    // RTC initialiseren
    if (!rtc.begin()) {
        Serial.println("RTC niet gevonden!");
        while (1);
    }

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.println("RTC & OLED actief!");
    oled.display();
    delay(2000);
}

void loop() {
    DateTime now = rtc.now();  // Haal huidige tijd op

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0, 0);
    oled.println("Tijd: ");
    
    oled.setCursor(0, 10);
    oled.print(now.hour(), DEC);
    oled.print(":");
    oled.print(now.minute(), DEC);
    oled.print(":");
    oled.print(now.second(), DEC);

    oled.display();
    delay(1000);
}
