#include "buttons.h"

volatile bool upTriggered = false;
volatile bool downTriggered = false;
volatile bool selectTriggered = false;

//for debouncing
static volatile unsigned long lastUp = 0;
static volatile unsigned long lastDown = 0;
static volatile unsigned long lastSelect = 0;

void IRAM_ATTR handleUp() {
    unsigned long currTime = millis();
    if (currTime - lastUp >= 200) {
        upTriggered = true;
        lastUp = currTime;
    }
}
void IRAM_ATTR handleDown() {
    unsigned long currTime = millis();
    if (currTime - lastDown >= 200) {
        downTriggered = true;
        lastDown = currTime;
    }
}
void IRAM_ATTR handleSelect() {
    unsigned long currTime = millis();
    if (currTime - lastSelect >= 200) {
        selectTriggered = true;
        lastSelect = currTime;
    }
}

void buttonsSetup() {
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);

    attachInterrupt(BTN_UP, handleUp, FALLING);
    attachInterrupt(BTN_DOWN, handleDown, FALLING);
    attachInterrupt(BTN_SELECT, handleSelect, FALLING);
}