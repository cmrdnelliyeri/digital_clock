#include <LiquidCrystal.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <string.h>

const int RS = 2, EN = 3, D4 = 6, D5 = 7, D6 = 8, D7 = 9;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
RTC_DS1307 rtc;

String daysOfTheWeek[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
String months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

String time_value = "";
String temperature = " 25.5*C";
String humidty = " 66.1%";
String date_value = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  
  lcd.begin(16,2);
  lcd.setCursor(0,0);

  #ifndef ESP8266
    while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    lcd.print("RTC ERR, Set Time");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));  

}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  lcd.setCursor(0,0);
  String date_time_rtc = get_date_time_from_rtc();
  String time_value = date_time_rtc.substring(0,9);
  lcd.print(time_value+temperature);
  String date_value = date_time_rtc.substring(10,date_time_rtc.length());
  lcd.setCursor(0,1);
  lcd.print(date_value+humidty);
  delay(1000);
  lcd.clear();

}

String get_date_time_from_rtc(){

  static bool second_pulse = true;

  DateTime now = rtc.now();
  String date_time = "";

  //Get time HH:MM::SS
  String now_hour = String(now.hour());
  if (now_hour.length() == 1)
    now_hour = "0" + now_hour;

  date_time += now_hour;

  String now_minute = String(now.minute());
  if (now_minute.length() == 1)
    now_minute = "0" + now_minute;

  if (second_pulse)
    date_time += ":" + now_minute;
  else
    date_time += " " + now_minute;

  second_pulse = !second_pulse;

  date_time += " " + daysOfTheWeek[now.dayOfTheWeek()];
  
  //Add split character 
  date_time += "/";

  //Get date 
  date_time += String(now.day());
  date_time += " " + months[now.month()-1];
  date_time += " " + String((now.year()%100));

  Serial.println(date_time);

  return date_time;

}
