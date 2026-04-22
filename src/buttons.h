#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

// Pin Definitions - Adjust these to your actual ESP32-C6 GPIOs
#define BTN_SHUFFLE 4
#define BTN_DEAL    5
#define BTN_LEFT    6
#define BTN_RIGHT   7

void buttonsSetup();
bool isShufflePressed();
bool isDealPressed();
bool isLeftPressed();
bool isRightPressed();

#endif