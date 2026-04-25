#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

// Pins for ESP32-C6
#define BTN_DEAL        18
#define BTN_LEFT        19
#define BTN_RIGHT       22
#define BTN_SPEED_UP    23
#define BTN_SPEED_DOWN  1   // pick a free GPIO

void buttonsSetup();

extern volatile bool dealTriggered;
extern volatile bool leftTriggered;
extern volatile bool rightTriggered;
extern volatile bool speedUpTriggered;
extern volatile bool speedDownTriggered;

#endif