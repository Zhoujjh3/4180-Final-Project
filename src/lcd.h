#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_I2C.h>

// Initialize LCD: Address 0x27 is common, 16 columns, 2 rows
void lcdSetup();
void displayStatus(const char* line1, const char* line2 = "");
void displayProgress(int current, int total);

#endif