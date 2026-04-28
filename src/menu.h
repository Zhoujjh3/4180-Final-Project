#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

extern unsigned int numPlayers;
extern unsigned int numCards;

enum screenStates {
    SCREEN_MENU,
    SCREEN_SET_PLAYERS,
    SCREEN_SET_CARDS,
    SCREEN_DEALING,
    SCREEN_SHUFFLING
};

enum menuStates {
    MENU_SHUFFLE,
    MENU_DEAL,
    MENU_PLAYERS,
    MENU_CARDS
};

extern screenStates currScreen;
extern menuStates currMenu;

void transitionScreen(boolean upTriggered, boolean downTriggered, boolean selectTriggered);
void transitionMenu(boolean upTriggered, boolean downTriggered, boolean selectTriggered);
void saveToNV();
void loadFromNV();

#endif