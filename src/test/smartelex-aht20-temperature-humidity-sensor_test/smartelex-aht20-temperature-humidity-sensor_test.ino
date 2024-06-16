#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

   if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }

  Serial.println("AHT20 found");

}

void loop() {
  // put your main code here, to run repeatedly:
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature);
  Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity);
  Serial.println("% rH");
  delay(500);
}
