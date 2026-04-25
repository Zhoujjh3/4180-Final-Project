#include "buttons.h"

volatile bool dealTriggered      = false;
volatile bool leftTriggered      = false;
volatile bool rightTriggered     = false;
volatile bool speedUpTriggered   = false;
volatile bool speedDownTriggered = false;

void IRAM_ATTR handleDeal()      { dealTriggered      = true; }
void IRAM_ATTR handleLeft()      { leftTriggered      = true; }
void IRAM_ATTR handleRight()     { rightTriggered     = true; }
void IRAM_ATTR handleSpeedUp()   { speedUpTriggered   = true; }
void IRAM_ATTR handleSpeedDown() { speedDownTriggered = true; }

void buttonsSetup() {
    pinMode(BTN_DEAL,       INPUT_PULLUP);
    pinMode(BTN_LEFT,       INPUT_PULLUP);
    pinMode(BTN_RIGHT,      INPUT_PULLUP);
    pinMode(BTN_SPEED_UP,   INPUT_PULLUP);
    pinMode(BTN_SPEED_DOWN, INPUT_PULLUP);

    attachInterrupt(BTN_DEAL,       handleDeal,      FALLING);
    attachInterrupt(BTN_LEFT,       handleLeft,      FALLING);
    attachInterrupt(BTN_RIGHT,      handleRight,     FALLING);
    attachInterrupt(BTN_SPEED_UP,   handleSpeedUp,   FALLING);
    attachInterrupt(BTN_SPEED_DOWN, handleSpeedDown, FALLING);
}