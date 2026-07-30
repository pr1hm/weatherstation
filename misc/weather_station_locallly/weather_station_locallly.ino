#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); 

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("{\"error\": \"Failed to read from sensor\"}");
    return;
  }

  Serial.print("{\"temperature\": \"");
  Serial.print(temp);
  Serial.print("\", \"humidity\": \"");
  Serial.print(hum);
  Serial.println("\"}");
}