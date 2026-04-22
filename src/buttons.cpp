#include "buttons.h"

volatile bool shuffleTriggered = false;
volatile bool dealTriggered    = false;
volatile bool toggleTriggered   = false;
volatile bool upTriggered       = false;

// ISRs with basic debouncing check
void IRAM_ATTR handleShuffle() { shuffleTriggered = true; }
void IRAM_ATTR handleDeal()    { dealTriggered = true; }
void IRAM_ATTR handleToggle()  { toggleTriggered = true; }
void IRAM_ATTR handleUp()      { upTriggered = true; }

void buttonsSetup() {
    pinMode(BTN_SHUFFLE, INPUT_PULLUP);
    pinMode(BTN_DEAL, INPUT_PULLUP);
    pinMode(BTN_TOGGLE, INPUT_PULLUP);
    pinMode(BTN_UP, INPUT_PULLUP);

    attachInterrupt(BTN_SHUFFLE, handleShuffle, FALLING);
    attachInterrupt(BTN_DEAL,    handleDeal,    FALLING);
    attachInterrupt(BTN_TOGGLE,  handleToggle,  FALLING);
    attachInterrupt(BTN_UP,      handleUp,      FALLING);
}