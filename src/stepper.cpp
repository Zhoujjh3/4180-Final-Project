// NOT USED

#include "stepper.h"

void stepperSetup() {
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
}

void move45Degrees() {
    digitalWrite(POWER_PIN, HIGH);
    delay(10); 

    digitalWrite(DIR_PIN, HIGH);
    
    // 45 degrees / 1.8 per step = 25 steps
    for (int i = 0; i < 25; i++) {
        digitalWrite(STEP_PIN, HIGH);
        delayMicroseconds(3000);
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(3000);
    }

    digitalWrite(POWER_PIN, LOW);
}