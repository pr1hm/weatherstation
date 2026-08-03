#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

// Sensor Settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- REPLACE THESE WITH YOUR DETAILS ---
const char* ssid = "WIFI/HOTSPOT_NAME";           // Your home network name
const char* password = "WIFI/HOTSPOT_PASS";   // Your Wi-Fi password
String apiKey = "<THINGSPEAK_WRITE_API_KEY>"; // The Write API Key from Step 5
// ---------------------------------------

const char* server = "http://api.thingspeak.com/update";

// Timer variables for non-blocking delays
unsigned long lastTime = 0;
unsigned long timerDelay = 20000; // 20 seconds

void setup() {
  // 1. Changed to 115200 (Standard for ESP32 boot logs)
  Serial.begin(115200); 
  dht.begin();

  connectToWiFi();
}

void loop() {
  // 2. Auto-Reconnect Logic
  // If connection is lost, it will continuously try to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection lost. Attempting to reconnect...");
    WiFi.disconnect();
    WiFi.reconnect();
    delay(5000); // Wait 5 seconds before retrying to prevent spamming
    return;      // Skip the rest of the loop until connected
  }

  // 3. Non-blocking timer using millis()
  // This allows the first reading to happen instantly on boot
  if ((millis() - lastTime) >= timerDelay || lastTime == 0) {
    
    // Read the Sensor
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(hum) || isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" °C, Humidity: ");
      Serial.print(hum);
      Serial.println(" %");

      // Send Data to the Cloud
      WiFiClient client;  
      HTTPClient http;
      
      String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temp) + "&field2=" + String(hum);
      
      // Open the connection using the client
      http.begin(client, url); 
      
      int httpResponseCode = http.GET();
      
      if (httpResponseCode > 0) {
        Serial.print("Data pushed! HTTP Response code: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Error pushing data. Error code: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
    }
    
    // Reset the timer
    lastTime = millis();
  }
}

// Separated Wi-Fi logic into a clean function
void connectToWiFi() {
  Serial.print("\nConnecting to Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Added a timeout so it doesn't get stuck in an infinite loop forever
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect. Will keep trying in the main loop.");
  }
}
