#include <LiquidCrystal.h>

const int RS = 2, EN = 3, D4 = 6, D5 = 7, D6 = 8, D7 = 9;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);

}

void loop() 
{
  // put your main code here, to run repeatedly:

  lcd.setCursor(0,0);
  lcd.print("Hello World");
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("Digital Clock");
  delay(2000);
  lcd.clear();
  delay(2000);
}
