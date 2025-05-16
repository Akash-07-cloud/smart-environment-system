/*****************************************************
 * SMART ENVIRONMENT MONITORING SYSTEM
 * -----------------------------------
 * - Monitors Temperature & Humidity using DHT11
 * - Sends data to ThingSpeak IoT Cloud
 * - WiFi enabled via ESP32
 *
 * Components:
 * - ESP32 Development Board
 * - DHT11 Sensor
 * - ThingSpeak Account with Write API Key
 *
 * Created by: Akash
 *****************************************************/

#include <WiFi.h>
#include <DHT.h>

// ---- WiFi Configuration ----
const char* ssid = "Akash";
const char* password = "aka@123";

// ---- ThingSpeak Configuration ----
const char* server = "api.thingspeak.com";
String apiKey = "SOVK5SB5EUGFTZDI"; // Replace with your Write API Key

// ---- DHT Sensor Configuration ----
#define DHTPIN 14       // GPIO pin where DHT11 is connected
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---- WiFi Client ----
WiFiClient client;

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to WiFi!");
  Serial.print("📶 IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if the sensor returned valid data
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ Failed to read from DHT11 sensor.");
    delay(5000);
    return;
  }

  // Print to Serial Monitor
  Serial.println("🌡️ Temperature: " + String(temperature) + " °C");
  Serial.println("💧 Humidity   : " + String(humidity) + " %");

  // Connect to ThingSpeak and send data
  if (client.connect(server, 80)) {
    String url = "/update?api_key=" + apiKey + "&field1=" + String(temperature) + "&field2=" + String(humidity);

    client.print("GET " + url + " HTTP/1.1\r\n");
    client.print("Host: " + String(server) + "\r\n");
    client.print("Connection: close\r\n\r\n");

    Serial.println("📤 Data sent to ThingSpeak");
  } else {
    Serial.println("❌ Failed to connect to ThingSpeak server.");
  }

  client.stop();       // Disconnect
  delay(16000);        // 16s delay (ThingSpeak limit is 15s for free users)
}
