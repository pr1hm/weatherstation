#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Sensor Settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- REPLACE THESE WITH YOUR DETAILS ---
const char* ssid = "Madhavi";           // Your home network name
const char* password = "Haribol108";   // Your Wi-Fi password
String apiKey = "CGURAAXTKRGPJN4B"; // The Write API Key from Step 5
// ---------------------------------------

const char* server = "http://api.thingspeak.com/update";

void setup() {
  Serial.begin(9600);
  dht.begin();

  // 1. Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait until connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Wi-Fi connected successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // ThingSpeak's free tier requires a minimum of 15 seconds between data pushes.
  // We will wait 20 seconds to be safe.
  delay(20000); 

  // 2. Read the Sensor
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C, Humidity: ");
  Serial.print(hum);
  Serial.println(" %");

  // 3. Send Data to the Cloud
  // 3. Send Data to the Cloud
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;  // <-- ADDED THIS
    HTTPClient http;
    
    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temp) + "&field2=" + String(hum);
    
    // Open the connection using the client
    http.begin(client, url); // <-- UPDATED THIS
    
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("Data pushed! HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error pushing data. Error code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  } else {
    Serial.println("Error: Wi-Fi disconnected.");
  }
}