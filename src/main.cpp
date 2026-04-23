#include <Arduino.h>
#include "buttons.h"
#include "lcd.h"

// Keep track of the last action for the display
String lastAction = "None";

void setup() {
    Serial.begin(115200);
    
    // Initialize peripherals
    lcdSetup();      // Initializes ST7735 via SPI [cite: 51]
    buttonsSetup();  // Attaches interrupts to pins 4, 5, 6, 7 [cite: 60, 62]
    
    displayStatus("READY", "Press a Button", ST77XX_WHITE);
    Serial.println("System initialized: Buttons and Display only.");
}

void loop() {
    // Check flags set by the Interrupt Service Routines (ISRs)
    if (shuffleTriggered) {
        shuffleTriggered = false; // Reset the flag
        lastAction = "SHUFFLE";
        displayStatus("ACTION", "SHUFFLING", ST77XX_ORANGE);
        Serial.println("Button: Shuffle Pressed");
    }

    if (dealTriggered) {
        dealTriggered = false;
        lastAction = "DEAL";
        displayStatus("ACTION", "DEALING", ST77XX_GREEN);
        Serial.println("Button: Deal Pressed");
    }

    if (toggleTriggered) {
        toggleTriggered = false;
        lastAction = "TOGGLE";
        displayStatus("MENU", "TOGGLED", ST77XX_YELLOW);
        Serial.println("Button: Toggle Pressed");
    }

    if (upTriggered) {
        upTriggered = false;
        lastAction = "UP";
        displayStatus("INPUT", "INCREASED", ST77XX_CYAN);
        Serial.println("Button: Up Pressed");
    }

    // Small delay to prevent the loop from spinning too fast
    delay(10); 
}