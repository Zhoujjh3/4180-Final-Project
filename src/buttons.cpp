#include "buttons.h"

void buttonsSetup() {
    pinMode(BTN_SHUFFLE, INPUT_PULLUP);
    pinMode(BTN_DEAL, INPUT_PULLUP);
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
}

// Returns true only if the button is currently pulled LOW (pressed)
bool isShufflePressed() { return digitalRead(BTN_SHUFFLE) == LOW; }
bool isDealPressed()    { return digitalRead(BTN_DEAL) == LOW; }
bool isLeftPressed()    { return digitalRead(BTN_LEFT) == LOW; }
bool isRightPressed()   { return digitalRead(BTN_RIGHT) == LOW; }