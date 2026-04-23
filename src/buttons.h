#define BUTTONS_H

#include <Arduino.h>

// Pins for ESP32-C6
#define BTN_SHUFFLE 18
#define BTN_DEAL    19
#define BTN_TOGGLE  22 // Switches between editing "Players" and "Cards"
#define BTN_UP      23 // Increments the selected value

void buttonsSetup();

// Flags for the main loop
extern volatile bool shuffleTriggered;
extern volatile bool dealTriggered;
extern volatile bool toggleTriggered;
extern volatile bool upTriggered;