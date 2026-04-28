#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

#define BTN_DOWN  18
#define BTN_UP    19
#define BTN_SELECT 20

void buttonsSetup();

extern volatile bool upTriggered;
extern volatile bool downTriggered;
extern volatile bool selectTriggered;

#endif