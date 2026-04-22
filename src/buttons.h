#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

// Pins for ESP32-C6
#define BTN_SHUFFLE 4
#define BTN_DEAL    5
#define BTN_TOGGLE  6 // Switches between editing "Players" and "Cards"
#define BTN_UP      7 // Increments the selected value

void buttonsSetup();

// Flags for the main loop
extern volatile bool shuffleTriggered;
extern volatile bool dealTriggered;
extern volatile bool toggleTriggered;
extern volatile bool upTriggered;

#endifv