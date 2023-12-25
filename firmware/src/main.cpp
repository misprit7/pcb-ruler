#include <Arduino.h>
#include "HID-APIs/ConsumerAPI.h"
#include "HID-Project.h"
#include "Adafruit_FreeTouch.h"
#include "adafruit_ptc.h"
#include "api/Common.h"
#include <math.h>

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

String keys[13] = {
    "c",
    "c#",
    "d",
    "d#",
    "e",
    "f",
    "f#",
    "g",
    "g#",
    "a",
    "a#",
    "b",
    "c",
};

int thresh[16] = {
    1200,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    1000,
    800,
    800,
    800,
};

int touch_baselines[16];

int held_dur[16] = {0};
bool sent[16] = {false};

double slider_pos = -1;
int slider_dir = 0;
double slider_len = 0;
int last_press = 0;

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
    SerialUSB.begin(115200);
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
    Keyboard.begin();
    Consumer.begin();
}

void loop()
{
    /* Keyboard.write('Y'); */
    /* delay(1000); */
    for(int i = 1; i < 16; ++i){
        touchin_reset();
        qt[i].begin(); 
        int result = qt[i].measure(); 
        SerialUSB.print(result);
        SerialUSB.print(", ");
        /* if(abs(result - touch_baselines[i]) > TOUCH_THRESH) { */
        if(result > 1000) {
            held_dur[i] = min(held_dur[i]+1, 5);

            /* SerialUSB.println(held_dur[i]); */
        } else {
            held_dur[i] = max(held_dur[i]-2, 0);
        }
    }

    bool key_pressed = false;
    for(int i = 0; i < 13; ++i){
        
        /* SerialUSB.print(held_dur[i]); */
        /* SerialUSB.print(", "); */

        if(!sent[i] && held_dur[i] >= 5){
            Keyboard.print(keys[i]);
            /* SerialUSB.println("Keyboard"); */
            sent[i] = true;
        } else if (held_dur[i] == 0){
            sent[i] = false;
        }

        if(held_dur[i] >= 5){
            key_pressed = true;
        }
    }

    bool slider[3] = {
        held_dur[13] >= 5,
        held_dur[14] >= 5,
        held_dur[15] >= 5,
    };
    int num_held = slider[0] + slider[1] + slider[2];

    if(num_held == 1 || num_held == 2){
        double slider_pos_new = (slider[0]*0 + slider[1]*1.0 + slider[2]*2.0)/num_held;
        if(slider[0] && slider[2]) slider_pos_new = 2.5;
        // No position yet
        if(slider_pos < 0){
            slider_pos = slider_pos_new;
        } else if(abs(slider_pos_new-slider_pos)>0.01){
            if(fmod(abs(slider_pos+0.5-slider_pos_new), 3) < 0.01 && slider_dir != -1){
                slider_dir = 1;
                slider_len += 0.5;
            } else if(fmod(abs(slider_pos-0.5-slider_pos_new), 3) < 0.01 && slider_dir != 1){
                slider_dir = -1;
                slider_len += 0.5;
            } else {
                slider_pos = -1;
                slider_dir = 0;
                slider_len = 0;
            }
        }
        slider_pos = slider_pos_new;
    } else {
        slider_pos = -1;
        slider_dir = 0;
        slider_len = 0;
    }

    if(slider_len > 0 && slider_len <=2){
        digitalWrite(LED_R, slider_dir == 1 ? LOW : HIGH);
        digitalWrite(LED_G, slider_dir == 1 ? HIGH : LOW);
        digitalWrite(LED_B, HIGH);
    } else if(slider_len > 2 && slider_len <=4){
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, LOW);
        if(millis() - last_press > 150){
            Consumer.write(slider_dir == 1 ? MEDIA_VOLUME_UP : MEDIA_VOLUME_DOWN);
            last_press = millis();
        }
    } else if(slider_len > 4){
        digitalWrite(LED_R, slider_dir == 1 ? HIGH : LOW);
        digitalWrite(LED_G, slider_dir == 1 ? LOW : HIGH);
        digitalWrite(LED_B, HIGH);
        if(millis() - last_press > 75){
            Consumer.write(slider_dir == 1 ? MEDIA_VOLUME_UP : MEDIA_VOLUME_DOWN);
            last_press = millis();
        }
    } else {
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_G, HIGH);
        digitalWrite(LED_B, HIGH);
    }

    /* SerialUSB.print(slider_pos); */
    /* SerialUSB.print(", "); */
    /* SerialUSB.print(slider_dir); */
    /* SerialUSB.print(", "); */
    /* SerialUSB.print(slider_len); */
    /* SerialUSB.print(", "); */


    /* SerialUSB.print(key_pressed); */
    digitalWrite(LED_W, key_pressed ? LOW : HIGH);

    SerialUSB.println();
    delay(5);
    /* delay(10); */
}

