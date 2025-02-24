extern "C" {
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
}

int freeMemory() {
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void setup() {
    Serial.begin(115200);
    Serial.print("Vrij geheugen: ");
    Serial.print(freeMemory());
    Serial.println(" bytes");
}

void loop() {
    // Je code hier
}
