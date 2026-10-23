#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
 
// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
 
// ThingSpeak credentials
unsigned long channelID = 1234567;
const char* writeAPIKey = "YOUR_WRITE_API_KEY";
 
// DHT11 configuration
#define DHTPIN 21
#define DHTTYPE DHT11
 
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
 
void setup() {
  Serial.begin(115200);
  delay(1000);
 
  Serial.println("ESP32 DHT11 ThingSpeak Logger");
  dht.begin();
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to Wi-Fi");
  int attempts = 0;
 
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
 
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed!");
  }
 
  ThingSpeak.begin(client);
}
 
void loop() {
  // Reconnect to Wi-Fi if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to Wi-Fi...");
    WiFi.reconnect();
 
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - startTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }
 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Upload skipped: Wi-Fi unavailable.");
    delay(10000);
    return;
  }
 
  // Read sensor measurements
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
 
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 reading failed! Check sensor wiring.");
    delay(2000);
    return;
  }
 
  // Display readings
  Serial.println("\n--- DHT11 Sensor Data ---");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" *C");
 
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");
 
  // Upload both measurements to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
 
  int status = ThingSpeak.writeFields(channelID, writeAPIKey);
 
  if (status == 200) {
    Serial.println("Upload successful!");
  } else {
    Serial.print("Upload failed. HTTP code: ");
    Serial.println(status);
  }
 
  Serial.println("-------------------------");
 
  // Wait 20 seconds between uploads
  delay(20000);
}
