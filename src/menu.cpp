
#include <Preferences.h>

#include "menu.h"
#include "lcd.h"
#include "buttons.h"

unsigned int numPlayers = 1;
unsigned int numCards = 1;

screenStates currScreen = SCREEN_MENU;
menuStates currMenu = MENU_CARDS;

Preferences prefs;

void transitionScreen(boolean upTriggered, boolean downTriggered, boolean selectTriggered) {
    switch(currScreen) {
        case SCREEN_MENU:
            transitionMenu(upTriggered, downTriggered, selectTriggered);
            break;
        case SCREEN_SET_CARDS:
            if (upTriggered && numCards < (52 / numPlayers)) {
                numCards++;
            } else if (downTriggered && numCards > 1) {
                numCards--;
            } else if (selectTriggered) {
                saveToNV();
                currScreen = SCREEN_MENU;
            }
            break;
        case SCREEN_SET_PLAYERS:
            if (upTriggered && numPlayers < 8) {
                numPlayers++;
            } else if (downTriggered && numPlayers > 0) {
                numPlayers--;
            } else if (selectTriggered) {
                saveToNV();
                currScreen = SCREEN_MENU;
            }
            break;
        case SCREEN_DEALING:
            if (selectTriggered) {
                currScreen = SCREEN_MENU;
            }
            break;
        case SCREEN_SHUFFLING:
            if (selectTriggered) {
                currScreen = SCREEN_MENU;   
            }
            break;
        case SCREEN_MANUAL_SHUFFLING:          // ← new
            if (selectTriggered) {
                currScreen = SCREEN_MENU;
            }
            break;
    }
    drawScreen(currScreen);
}

void transitionMenu(boolean upTriggered, boolean downTriggered, boolean selectTriggered) {
    switch(currMenu) {
        case MENU_CARDS:
            if (upTriggered) {
                currMenu = MENU_SHUFFLE;
            } else if (downTriggered) {
                currMenu = MENU_PLAYERS;
            } else if (selectTriggered) {
                currScreen = SCREEN_SET_CARDS;
            }
            break;
        case MENU_PLAYERS:
            if (upTriggered) {
                currMenu = MENU_CARDS;
            } else if (downTriggered) {
                currMenu = MENU_DEAL;
            } else if (selectTriggered) {
                currScreen = SCREEN_SET_PLAYERS;
            }
            break;
        case MENU_DEAL:
            if (upTriggered) {
                currMenu = MENU_PLAYERS;
            } else if (downTriggered) {
                currMenu = MENU_SHUFFLE;
            } else if (selectTriggered) {
                currScreen = SCREEN_DEALING;
            }
            break;
        case MENU_SHUFFLE:
            if (upTriggered) {
                currMenu = MENU_DEAL;
            } else if (downTriggered) {
                currMenu = MENU_MANUAL_SHUFFLE; 
            } else if (selectTriggered) {
                currScreen = SCREEN_SHUFFLING;
            }
            break;
        case MENU_MANUAL_SHUFFLE:              
            if (upTriggered) {
                currMenu = MENU_SHUFFLE;
            } else if (downTriggered) {
                currMenu = MENU_CARDS;        
            } else if (selectTriggered) {
                currScreen = SCREEN_MANUAL_SHUFFLING;
            }
            break;
    }
    drawArrow(currMenu);

}

void saveToNV() {
  prefs.begin("dealer", false);
  prefs.putUInt("numCards", numCards);
  prefs.putUInt("numPlayers", numPlayers);
  prefs.end();
}

void loadFromNV() {                                           
      prefs.begin("dealer", true);
      numCards   = prefs.getUInt("numCards", 1);               
      numPlayers = prefs.getUInt("numPlayers", 2);             
      prefs.end();                                           
  }      


