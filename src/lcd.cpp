#include "lcd.h"
#include "menu.h"
#include <stdio.h>

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// constants
#define MENU_TEXT_X   22
// y positions for each menu item
// MENU_SHUFFLE=0, MENU_DEAL=1, MENU_PLAYERS=2, MENU_CARDS=3
static const int MENU_Y[] = { 35, 53, 71, 89 };

// internal states
static screenStates _lastScreen = (screenStates)-1;  // force fresh draw on first call
static int _prevArrowMenu = -1;

static void drawMenu();
static void drawNumberScreen(const char* label, unsigned int value, bool fresh);

void lcdSetup() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillRect(0, 0, 160, 128, ST77XX_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Card Dealer");
    tft.setTextSize(1);
    tft.println("System Ready...");
}

// dealing/shuffling screens
void displayStatus(const char* title, const char* value, uint16_t color) {
    tft.fillRect(0, 10, tft.width(), 25, ST77XX_BLACK);
    tft.fillRect(0, 50, tft.width(), 25, ST77XX_BLACK);

    tft.setCursor(10, 20);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println(title);

    tft.setCursor(10, 60);
    tft.setTextColor(color);
    tft.setTextSize(2);
    tft.println(value);
}

// draws a new triangle
void drawArrow(menuStates m) {
    // Erase previous arrow
    if (_prevArrowMenu >= 0) {
        int oldY = MENU_Y[_prevArrowMenu];
        tft.fillTriangle(5, oldY, 5, oldY + 8, 13, oldY + 4, ST77XX_BLACK);
    }
    // Draw new arrow (right-pointing triangle)
    int y = MENU_Y[m];
    tft.fillTriangle(5, y, 5, y + 8, 13, y + 4, ST77XX_WHITE);
    _prevArrowMenu = (int)m;
}

// updates the screen
void drawScreen(screenStates s) {
    bool fresh = (s != _lastScreen);
    _lastScreen = s;

    switch (s) {
        case SCREEN_MENU:
            if (fresh) {
                _prevArrowMenu = -1;
                drawMenu();
            }
            // Arrow is already handled by drawArrow() in transitionMenu
            break;
        case SCREEN_SET_PLAYERS:
            drawNumberScreen("PLAYERS", numPlayers, fresh);
            break;
        case SCREEN_SET_CARDS:
            drawNumberScreen("Set Deal AMT", numCards, fresh);
            break;
        case SCREEN_DEALING:
            if (fresh) {
                tft.fillScreen(ST77XX_BLACK);
                displayStatus("DEAL", "Dealing...", ST77XX_GREEN);
            }
            break;
        case SCREEN_SHUFFLING:
            if (fresh) {
                tft.fillScreen(ST77XX_BLACK);
                displayStatus("SHUFFLE", "Shuffling...", ST77XX_YELLOW);
            }
            break;
    }
}

// draws the full main menu — title + 4 items + arrow
static void drawMenu() {
    tft.fillScreen(ST77XX_BLACK);

    // title
    tft.setCursor(10, 5);
    tft.setTextColor(ST77XX_CYAN);
    tft.setTextSize(2);
    tft.println("GT DEALER");

    // menu items
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(MENU_TEXT_X, MENU_Y[MENU_SHUFFLE]); tft.println("SHUFFLE");
    tft.setCursor(MENU_TEXT_X, MENU_Y[MENU_DEAL]);    tft.println("DEAL");
    tft.setCursor(MENU_TEXT_X, MENU_Y[MENU_PLAYERS]); tft.println("Set Player CNT");
    tft.setCursor(MENU_TEXT_X, MENU_Y[MENU_CARDS]);   tft.println("Set Deal AMT");

    // draw arrow at current position
    drawArrow(currMenu);
}

// Shows a label at the top and a large centered number
// fresh=true does a full redraw; fresh=false only updates the number=
static void drawNumberScreen(const char* label, unsigned int value, bool fresh) {
    if (fresh) {
        tft.fillScreen(ST77XX_BLACK);

        // Label
        tft.setCursor(10, 10);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.println(label);

        // Hint at bottom
        tft.setTextSize(1);
        tft.setTextColor(ST77XX_WHITE);
        tft.setCursor(10, tft.height() - 10);
        tft.println("UP/DOWN adjust  SEL back");
    }

    // Clear only the number region and redraw
    tft.fillRect(0, 40, tft.width(), 50, ST77XX_BLACK);

    char buf[4];
    snprintf(buf, sizeof(buf), "%u", value);
    int digits = strlen(buf);
    int textW = digits * 24;  // textSize 4: each char is 6*4=24px wide
    int x = (tft.width() - textW) / 2;
    tft.setCursor(x, 52);
    tft.setTextSize(4);
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(buf);
}

// old functions
void resetDealerDisplay() {}
void updateDealer(bool dealing, bool shuffling) {}
