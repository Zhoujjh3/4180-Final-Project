#include "lcd.h"

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ── Helper: overwrite a text region with black then redraw ─
static void clearRegion(int x, int y, int w, int h) {
    tft.fillRect(x, y, w, h, ST77XX_BLACK);
}


void lcdSetup() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // Fix white border pixels on ST7735
    tft.fillRect(0, 0, tft.width(), tft.height(), ST77XX_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println("Card Dealer");
    tft.setTextSize(1);
    tft.println("System Ready...");
}

void displayStatus(const char* title, const char* value, uint16_t color) {
    tft.fillScreen(ST77XX_BLACK);  // only called once on mode change, ok to full clear

    tft.setCursor(10, 20);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.println(title);

    tft.setCursor(10, 60);
    tft.setTextColor(color);
    tft.setTextSize(3);
    tft.println(value);
}

static int   _prevSpeed   = -1;
static int   _prevLeft    = -1;
static int   _prevRight   = -1;
static int   _prevDealing = -1;
static bool  _initialized = false;  // ← add this


void resetDealerDisplay() {
    _initialized = false;
}
void updateDealer(int speed, bool leftOn, bool rightOn, bool dealing) {

    // ── First call after BLE/status screen — full clear ────
    if (!_initialized) {
        tft.fillScreen(ST77XX_BLACK);
        _initialized = true;
        // Force all regions to redraw
        _prevSpeed   = -1;
        _prevLeft    = -1;
        _prevRight   = -1;
        _prevDealing = -1;
    }

    // ── Speed line ─────────────────────────────────────────
    if (speed != _prevSpeed) {
        clearRegion(5, 5, tft.width() - 5, 20);
        tft.setCursor(5, 5);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.print("Speed: ");
        tft.println(speed);
        _prevSpeed = speed;
    }

    // ── Left motor ─────────────────────────────────────────
    if ((int)leftOn != _prevLeft) {
        clearRegion(5, 40, tft.width() - 5, 20);
        tft.setCursor(5, 40);
        tft.setTextColor(leftOn ? ST77XX_GREEN : ST77XX_RED);
        tft.setTextSize(2);
        tft.println(leftOn ? "LEFT:  ON " : "LEFT:  OFF");
        _prevLeft = (int)leftOn;
    }

    // ── Right motor ────────────────────────────────────────
    if ((int)rightOn != _prevRight) {
        clearRegion(5, 70, tft.width() - 5, 20);
        tft.setCursor(5, 70);
        tft.setTextColor(rightOn ? ST77XX_GREEN : ST77XX_RED);
        tft.setTextSize(2);
        tft.println(rightOn ? "RIGHT: ON " : "RIGHT: OFF");
        _prevRight = (int)rightOn;
    }

    // ── Deal banner ────────────────────────────────────────
    if ((int)dealing != _prevDealing) {
        clearRegion(5, 105, tft.width() - 5, 20);
        tft.setCursor(5, 105);
        tft.setTextSize(2);
        tft.setTextColor(dealing ? ST77XX_YELLOW : ST77XX_CYAN);
        tft.println(dealing ? ">> DEALING <<" : "   READY    ");
        _prevDealing = (int)dealing;
    }
}