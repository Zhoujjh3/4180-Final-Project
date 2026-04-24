#include <Arduino.h>
#include "lcd.h"
#include "buttons.h"
#include "stepper.h"

// State variables for the card dealer logic
int numPlayers = 2;
int numCards = 5;
bool editingPlayers = true; // Toggle state

void setup() {
    Serial.begin(115200);
    
    // Initialize hardware modules
    lcdSetup();
    stepperSetup();
    buttonsSetup();

    // Initial splash screen and menu
    displayStatus("WELCOME", "GT Card Dealer", ST77XX_CYAN);
    delay(1500);
    updateMenu(numPlayers, numCards, editingPlayers);
    
    Serial.println("System Initialized and Ready.");
}

void loop() {
    // 1. SHUFFLE: Move motor and update screen
    if (shuffleTriggered) {
        displayStatus("ACTION", "Shuffling...", ST77XX_YELLOW);
        move45Degrees();
        shuffleTriggered = false;
        updateMenu(numPlayers, numCards, editingPlayers);
    }

    // 2. DEAL: Move motor and update screen
    if (dealTriggered) {
        displayStatus("ACTION", "Dealing...", ST77XX_GREEN);
        move45Degrees();
        dealTriggered = false;
        updateMenu(numPlayers, numCards, editingPlayers);
    }

    // 3. TOGGLE: Switch between editing Players or Cards
    if (toggleTriggered) {
        editingPlayers = !editingPlayers;
        displayStatus("MODE", editingPlayers ? "Edit Players" : "Edit Cards", ST77XX_MAGENTA);
        move45Degrees(); // Move 45 deg for every button as requested
        toggleTriggered = false;
        delay(500); // Small delay to show mode change
        updateMenu(numPlayers, numCards, editingPlayers);
    }

    // 4. UP: Increment the selected value
    if (upTriggered) {
        if (editingPlayers) {
            numPlayers = (numPlayers % 8) + 1; // Cycle 1-8
        } else {
            numCards = (numCards % 52) + 1;    // Cycle 1-52
        }
        
        displayStatus("UP", "+1 Increment", ST77XX_WHITE);
        move45Degrees(); 
        upTriggered = false;
        updateMenu(numPlayers, numCards, editingPlayers);
    }
}