#ifndef LCD_H
#define LCD_H

#include <Adafruit_GFX.h>    
#include <Adafruit_ST7735.h> 
#include <SPI.h>

// ESP32-C6 SPI Pins (Adjust based on your wiring)
#define TFT_CS    10
#define TFT_RST  -1  // Or connect to ESP32 Reset
#define TFT_DC    2
#define TFT_MOSI  7  // Standard SDA
#define TFT_SCLK  6  // Standard SCK

void lcdSetup();
void displayStatus(const char* title, const char* value, uint16_t color);
void updateMenu(int players, int cards, bool editingPlayers);
void updateDealer(int speed, bool leftOn, bool rightOn, bool dealing);

#endif