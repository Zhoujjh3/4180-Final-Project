#include "lcd.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void lcdSetup() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Ready");
}

void displayStatus(const char* line1, const char* line2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void displayProgress(int current, int total) {
    lcd.setCursor(0, 1);
    lcd.print("Card: ");
    lcd.print(current);
    lcd.print("/");
    lcd.print(total);
}