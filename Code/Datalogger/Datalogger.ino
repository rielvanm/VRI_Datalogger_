#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED-configuratie
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin niet nodig bij I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RTC-module
RTC_DS3231 rtc;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // OLED-initialisatie
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 0x3C is het standaard I2C-adres
        Serial.println(F("OLED niet gevonden!"));
        while (1);
    }
    display.clearDisplay();
    
    // RTC-initialisatie
    if (!rtc.begin()) {
        Serial.println("RTC niet gevonden!");
        displayText("RTC niet gevonden!", true);
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC heeft reset ondergaan, instellen op huidige tijd...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Stelt RTC in op de compilatietijd
    }

    Serial.println("RTC en OLED succesvol geïnitieerd.");
    displayText("RTC OK - Tijd uitlezen...", true);
    delay(2000);
}

void loop() {
    DateTime now = rtc.now();

    // Tijd als string genereren
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

    // Weergave op OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.print(buffer);
    display.display();

    // Seriële output
    Serial.print("Huidige tijd: ");
    Serial.println(buffer);

    delay(1000); // Update elke seconde
}

// Functie om tekst op het OLED-scherm weer te geven
void displayText(const char* text, bool clear) {
    if (clear) display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.println(text);
    display.display();
}
