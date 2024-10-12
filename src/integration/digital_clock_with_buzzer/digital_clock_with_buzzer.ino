#include <LiquidCrystal.h>
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <Adafruit_AHTX0.h>
#include <string.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

#define BIRTHDAY "11:26"

const int RS = 2, EN = 3, D4 = 6, D5 = 7, D6 = 8, D7 = 9;

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
RTC_DS1307 rtc;
Adafruit_AHTX0 aht;

String daysOfTheWeek[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
String months[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

String time_value = "";
String date_value = "";
sensors_event_t humidity, temperature;
bool rtc_running = false;
bool aht_20_running = false;

/** HBD Music setup starts**/

// change this to make the song slower or faster
int tempo = 140;

// change this to whichever pin you want to use
int buzzer = 4;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {

  // Happy Birthday
  // Score available at https://musescore.com/user/8221/scores/26906

  NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_F4,-4,
  NOTE_E4,-2, NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_G4,-4,
  NOTE_F4,-2, NOTE_C4,4, NOTE_C4,8,

  NOTE_C5,-4, NOTE_A4,-4, NOTE_F4,-4, 
  NOTE_E4,-4, NOTE_D4,-4, NOTE_AS4,4, NOTE_AS4,8,
  NOTE_A4,-4, NOTE_F4,-4, NOTE_G4,-4,
  NOTE_F4,-2,
 
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

bool is_birthday = false;

/** HBD Music setup ends**/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  pinMode(buzzer, OUTPUT);
  
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

  lcd.print("Checking RTC..");
  delay(1000);
  lcd.clear();

  if (! rtc.isrunning()) {
    lcd.print("RTC ERR, Set Time");
    delay(2000);
    lcd.clear();
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  rtc_running = true;
  lcd.print("RTC running..");
  delay(1000);
  lcd.clear();

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); 

  lcd.print("Checking AHT20..");
  delay(1000);
  lcd.clear();   

  if (! aht.begin()) {
    lcd.print("RTC ERR, Set Time");
    delay(2000);
    lcd.clear();
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  aht_20_running = true;
  lcd.print("AHT20 running..");
  delay(1000);
  lcd.clear();

  lcd.print("Andromeda Labs");
  lcd.setCursor(0,1);
  lcd.print("LOADING !!!");

  startup_sound();

  delay(3000);
  lcd.clear();

}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  lcd.clear();
  lcd.setCursor(0,0);
  aht.getEvent(&humidity, &temperature);// populate temp and humidity objects with fresh data
  String date_time_rtc = get_date_time_from_rtc();
  Serial.print(date_time_rtc);
  String time_value = date_time_rtc.substring(0,9);
  lcd.print(time_value+ " " + String(temperature.temperature).substring(0,4));
  lcd.setCursor(14,0);
  lcd.print((char)223);
  lcd.setCursor(15,0);
  lcd.print("C");
  String date_value = date_time_rtc.substring(10,date_time_rtc.length());
  lcd.setCursor(0,1);
  lcd.print(date_value + " " + String(humidity.relative_humidity).substring(0,4) + "%");

  Serial.print("Temperature: "); Serial.print(temperature.temperature);
  Serial.print(" degrees C ");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity);
  Serial.println("% rH");

  is_birthday =  check_birthday(date_time_rtc);

  delay(1000);  

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

  return date_time;

}

void startup_sound(){

  for(int i=0; i<4; i++){
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(500);
  }

}

void happy_birthday_music(){

  // iterate over the notes of the melody.
  // Remember, the array is twice the number of notes (notes + durations)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {

    // calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer, melody[thisNote], noteDuration * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration);

    // stop the waveform generation before the next note.
    noTone(buzzer);

  }

}

bool check_birthday(String date_time_rtc){

  return false;
}