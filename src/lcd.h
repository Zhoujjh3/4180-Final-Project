#ifndef LCD_H
#define LCD_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS    10
#define TFT_RST   -1
#define TFT_DC    2
#define TFT_MOSI  7
#define TFT_SCLK  6

extern Adafruit_ST7735 tft;

void lcdSetup();
void displayStatus(const char* title, const char* value, uint16_t color);
void resetDealerDisplay();
void updateDealer(bool dealing, bool shuffling);

#endif