#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

#define BTN_SHUFFLE  18
#define BTN_DEAL     19

void buttonsSetup();

extern volatile bool shuffleTriggered;
extern volatile bool dealTriggered;

#endif