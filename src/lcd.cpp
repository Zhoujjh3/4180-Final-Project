#include "lcd.h"

// USE THE FULL CONSTRUCTOR: CS, DC, MOSI (SDA), SCLK (SCL), RST
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void lcdSetup() {
    // Initialize ST7735R chip, black tab
    tft.initR(INITR_BLACKTAB); 
    tft.setRotation(1); // Landscape mode
    tft.fillScreen(ST77XX_BLACK);
    
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

void updateMenu(int players, int cards, bool editingPlayers) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextSize(2);
    
    // Highlight the active selection
    tft.setCursor(5, 20);
    tft.setTextColor(editingPlayers ? ST77XX_YELLOW : ST77XX_WHITE);
    tft.print("Players: ");
    tft.println(players);
    
    tft.setCursor(5, 60);
    tft.setTextColor(!editingPlayers ? ST77XX_YELLOW : ST77XX_WHITE);
    tft.print("Cards:   ");
    tft.println(cards);
}

void updateDealer(int speed, bool leftOn, bool rightOn, bool dealing) {
    tft.fillScreen(ST77XX_BLACK);

    // Speed bar at top
    tft.setCursor(5, 5);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print("Speed: ");
    tft.println(speed);

    // Left motor status
    tft.setCursor(5, 40);
    tft.setTextColor(leftOn ? ST77XX_GREEN : ST77XX_RED);
    tft.println(leftOn  ? "LEFT:  ON " : "LEFT:  OFF");

    // Right motor status
    tft.setCursor(5, 70);
    tft.setTextColor(rightOn ? ST77XX_GREEN : ST77XX_RED);
    tft.println(rightOn ? "RIGHT: ON " : "RIGHT: OFF");

    // Deal status banner
    tft.setCursor(5, 105);
    tft.setTextSize(2);
    tft.setTextColor(dealing ? ST77XX_YELLOW : ST77XX_CYAN);
    tft.println(dealing ? ">> DEALING <<" : "   READY    ");
}