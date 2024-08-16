#include <LiquidCrystal.h>
#include <string.h>

const int RS = 2, EN = 3, D4 = 6, D5 = 7, D6 = 8, D7 = 9;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

String time_value = "13:05 WED";
String temperature = " 25.5*C";
String humidty = " 66.1%";
String date_value = "16 AUG 24";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);

}

void loop() 
{
  // put your main code here, to run repeatedly:

  lcd.setCursor(0,0);
  lcd.print(time_value+temperature);
  lcd.setCursor(0,1);
  lcd.print(date_value+humidty);
  delay(1000);
  lcd.clear();
}
