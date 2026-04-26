#include "lcd.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

static void clearRegion(int x, int y, int w, int h) {
    tft.fillRect(x, y, w, h, ST77XX_BLACK);
}

static bool _initialized   = false;
static int  _prevDealing   = -1;
static int  _prevShuffling = -1;

void lcdSetup() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillRect(0, 0, 160, 128, ST77XX_BLACK);  // clears white border pixels

    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Card Dealer");
    tft.setTextSize(1);
    tft.println("System Ready...");
}

void displayStatus(const char* title, const char* value, uint16_t color) {
    tft.fillScreen(ST77XX_BLACK);

    tft.setCursor(10, 20);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println(title);

    tft.setCursor(10, 60);
    tft.setTextColor(color);
    tft.setTextSize(3);
    tft.println(value);
}

void resetDealerDisplay() {
    _initialized   = false;
    _prevDealing   = -1;
    _prevShuffling = -1;
}

void updateDealer(bool dealing, bool shuffling) {
    // Full clear only on first call after a status screen
    if (!_initialized) {
        tft.fillScreen(ST77XX_BLACK);
        _initialized = true;
    }

    // Only redraw shuffle row if it changed
    if ((int)shuffling != _prevShuffling) {
        clearRegion(5, 20, tft.width() - 5, 20);
        tft.setCursor(5, 20);
        tft.setTextSize(2);
        tft.setTextColor(shuffling ? ST77XX_YELLOW : ST77XX_WHITE);
        tft.println(shuffling ? "SHUFFLE: ON " : "SHUFFLE: OFF");
        _prevShuffling = (int)shuffling;
    }

    // Only redraw deal row if it changed
    if ((int)dealing != _prevDealing) {
        clearRegion(5, 60, tft.width() - 5, 20);
        tft.setCursor(5, 60);
        tft.setTextSize(2);
        tft.setTextColor(dealing ? ST77XX_GREEN : ST77XX_RED);
        tft.println(dealing ? "DEAL:    ON " : "DEAL:    OFF");
        _prevDealing = (int)dealing;
    }
}