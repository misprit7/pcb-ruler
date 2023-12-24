#include <Arduino.h>
#include "HID-Project.h"
#include "Adafruit_FreeTouch.h"
#include "adafruit_ptc.h"

#define LED_W 4
#define LED_R 3
#define LED_G 1
#define LED_B 0

#define TOUCH_OVERSAMPLE OVERSAMPLE_4
#define TOUCH_RES RESISTOR_0
#define TOUCH_FREQ FREQ_MODE_NONE

Adafruit_FreeTouch qt[16] = {
    Adafruit_FreeTouch(PORTA, 2, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTA, 3, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTA, 4, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTA, 5, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTA, 6, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTA, 7, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 0, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 1, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 2, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 3, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 4, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 5, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 6, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 7, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 8, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
    Adafruit_FreeTouch(PORTB, 9, TOUCH_OVERSAMPLE, TOUCH_RES, TOUCH_FREQ),
};

int touch_baselines[16];

// https://github.com/adafruit/circuitpython/blob/main/ports/atmel-samd/common-hal/touchio/TouchIn.c#L114
void touchin_reset() {
    Ptc *ptc = ((Ptc *)PTC);
    if (ptc->CTRLA.bit.ENABLE == 1) {
        ptc->CTRLA.bit.ENABLE = 0;
        while (ptc->CTRLA.bit.ENABLE == 1) {
        }

        ptc->CTRLA.bit.SWRESET = 1;
        while (ptc->CTRLA.bit.SWRESET == 1) {
        }
    }
}

void setup(){
    pinMode(LED_W, OUTPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    digitalWrite(LED_W, LOW);
    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
    SerialUSB.begin(9600);
    SerialUSB.println("Starting!");

    for(int i = 0; i < 16; ++i){
        if (!qt[i].begin())  
            SerialUSB.println("Failed to begin qt on pin A0");
    }

    for(int i = 0; i < 16; ++i){
        touchin_reset();
        qt[i].begin(); 
        int result = qt[i].measure(); 
        touch_baselines[i] = result;
    }
    /* while(!SerialUSB.available()); */
    /* Keyboard.begin(); */
}

void loop()
{
    for(int i = 0; i < 16; ++i){
        touchin_reset();
        qt[i].begin(); 
        int result = qt[i].measure(); 
        SerialUSB.print(result);
        SerialUSB.print(", ");
    }
    SerialUSB.println();
    delay(100);
}

