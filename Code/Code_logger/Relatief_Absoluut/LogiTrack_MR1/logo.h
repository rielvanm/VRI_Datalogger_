#ifndef LOGO_H
#define LOGO_H

/**
 * @file logo.h
 * @brief Declaration of a 128x64 monochrome bitmap for display on OLED.
 * 
 * This header declares the external Arduino logo bitmap array,
 * which is stored in PROGMEM in a corresponding .cpp file (typically logo.cpp).
 * 
 * The bitmap can be displayed using functions like drawBitmap() from
 * libraries such as Adafruit_SSD1306 or U8g2.
 * 
 * Example usage:
 *   display.drawBitmap(0, 0, arduino_icon, 128, 64, WHITE);
 */

/// @brief Extern declaration of 128x64 Arduino logo bitmap.
extern const unsigned char arduino_icon[];

#endif
