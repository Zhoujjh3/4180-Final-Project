#include "buttons.h"

volatile bool shuffleTriggered = false;
volatile bool dealTriggered = false;

//for debouncing
static volatile unsigned long lastShuffle = 0;
static volatile unsigned long lastDeal = 0;

void IRAM_ATTR handleShuffle() {
    unsigned long currTime = millis();
    if (currTime - lastShuffle >= 200) {
        shuffleTriggered = true;
        lastShuffle = currTime;
    }
}
void IRAM_ATTR handleDeal() {
     unsigned long currTime = millis();
    if (currTime - lastDeal >= 200) {
        dealTriggered = true;
        lastDeal = currTime;
    }
}

void buttonsSetup() {
    pinMode(BTN_SHUFFLE, INPUT_PULLUP);
    pinMode(BTN_DEAL, INPUT_PULLUP);

    attachInterrupt(BTN_SHUFFLE, handleShuffle, FALLING);
    attachInterrupt(BTN_DEAL, handleDeal, FALLING);
}