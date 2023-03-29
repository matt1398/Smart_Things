#define LCD_ADDRESS 0x27
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(LCD_ADDRESS, 2, 1, 0, 4, 5, 6, 7);
void setup() {
    lcd.begin(16, 2);
    lcd.setBacklightPin(3, POSITIVE);
    lcd.setBacklight(HIGH);
}
void loop() {
    lcd.home();
    lcd.print("SerialLCD ESP D1");
    lcd.setCursor(0, 1);
    lcd.print("---------------------");
    delay(1000);
}