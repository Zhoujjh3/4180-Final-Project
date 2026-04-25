#include <Arduino.h>
#include "lcd.h"
#include "buttons.h"
// #include "motors.h"   // ← commented out until motors are wired

#define SPEED_STEP        25
#define SPEED_MIN         50
#define SPEED_MAX         255
#define DEAL_DURATION_MS  600

int  currentSpeed   = 150;
bool leftMotorOn    = false;
bool rightMotorOn   = false;
bool isDealing      = false;
unsigned long dealStart = 0;

void applyMotors() {
    // motor1Speed(leftMotorOn  ? currentSpeed : 0);
    // motor2Speed(rightMotorOn ? currentSpeed : 0);
}

void refreshDisplay() {
    updateDealer(currentSpeed, leftMotorOn, rightMotorOn, isDealing);
}

void setup() {
    Serial.begin(115200);
    lcdSetup();
    // motorSetup();   // ← commented out
    buttonsSetup();

    displayStatus("GT DEALER", "Ready", ST77XX_CYAN);
    delay(1500);
    refreshDisplay();
    Serial.println("Ready.");
}

void loop() {

    if (leftTriggered) {
        leftTriggered = false;
        leftMotorOn = !leftMotorOn;
        applyMotors();
        Serial.printf("LEFT: %s\n", leftMotorOn ? "ON" : "OFF");
        refreshDisplay();
    }

    if (rightTriggered) {
        rightTriggered = false;
        rightMotorOn = !rightMotorOn;
        applyMotors();
        Serial.printf("RIGHT: %s\n", rightMotorOn ? "ON" : "OFF");
        refreshDisplay();
    }

    if (speedUpTriggered) {
        speedUpTriggered = false;
        currentSpeed = min(currentSpeed + SPEED_STEP, SPEED_MAX);
        Serial.printf("SPEED UP: %d\n", currentSpeed);
        refreshDisplay();
    }

    if (speedDownTriggered) {
        speedDownTriggered = false;
        currentSpeed = max(currentSpeed - SPEED_STEP, SPEED_MIN);
        Serial.printf("SPEED DOWN: %d\n", currentSpeed);
        refreshDisplay();
    }

    if (dealTriggered && !isDealing) {
        dealTriggered = false;
        isDealing  = true;
        dealStart  = millis();
        // motor1Speed(currentSpeed);
        // motor2Speed(currentSpeed);
        Serial.println("DEAL: Dealing...");
        refreshDisplay();
    }

    if (isDealing && (millis() - dealStart >= DEAL_DURATION_MS)) {
        isDealing = false;
        applyMotors();
        Serial.println("DEAL: Done.");
        refreshDisplay();
    }
}