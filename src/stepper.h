#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>

#define DIR_PIN    12
#define STEP_PIN   13
#define POWER_PIN  20 // SLEEP + RESET bridged

void stepperSetup();
void move45Degrees();

#endif