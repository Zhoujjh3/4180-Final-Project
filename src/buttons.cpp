#include "buttons.h"

volatile bool shuffleTriggered = false;
volatile bool dealTriggered    = false;

void IRAM_ATTR handleShuffle() { shuffleTriggered = true; }
void IRAM_ATTR handleDeal()    { dealTriggered    = true; }

void buttonsSetup() {
    pinMode(BTN_SHUFFLE, INPUT_PULLUP);
    pinMode(BTN_DEAL,    INPUT_PULLUP);

    attachInterrupt(BTN_SHUFFLE, handleShuffle, FALLING);
    attachInterrupt(BTN_DEAL,    handleDeal,    FALLING);
}