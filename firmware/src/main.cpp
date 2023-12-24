#include <Arduino.h>

#define PIN 14

void setup(){
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, LOW);
    /* Serial.begin(9600); */
    /* Serial.println("Starting!"); */
}

void loop()
{
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(PIN, HIGH);
    /* Serial.println("Blink!"); */

    // wait for a second
    delay(1000);

    /* Serial.println("Blink!"); */
    // turn the LED off by making the voltage LOW
    digitalWrite(PIN, LOW);

   // wait for a second
    delay(1000);
}

