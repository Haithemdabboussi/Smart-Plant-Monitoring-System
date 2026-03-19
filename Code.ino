/* Comment this out to disable Serial prints and save memory */
#define BLYNK_PRINT Serial

/* Blynk Template configuration (replace with your own credentials) */
#define BLYNK_TEMPLATE_ID   "TMPxxxxxx"
#define BLYNK_TEMPLATE_NAME "Device"
#define BLYNK_AUTH_TOKEN    "YourAuthToken"

/* Include required libraries */
#include <WiFi.h>              // WiFi library for ESP32
#include <WiFiClient.h>        // WiFi client support
#include <BlynkSimpleEsp32.h>  // Blynk library for ESP32
#include <DHT.h>               // DHT sensor library

/* DHT sensor configuration */
#define DHTPIN 2        // GPIO pin connected to DHT11 data pin
#define DHTTYPE DHT11   // Define sensor type (DHT11)

DHT dht(DHTPIN, DHTTYPE);  // Create DHT sensor object

/* Soil moisture sensor configuration */
#define SOIL_MOISTURE_PIN A0  // Analog pin connected to soil moisture sensor

/* WiFi credentials (replace with your network details) */
char ssid[] = "YourSSID";
char pass[] = "YourPassword";

/* Blynk authentication token */
char auth[] = BLYNK_AUTH_TOKEN;

/* Create Blynk timer object */
BlynkTimer timer;

/* Function to read sensors and send data to Blynk */
void sendSensorData()
{
  // Read humidity and temperature from DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read soil moisture analog value
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // Check if DHT reading failed
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send data to Blynk virtual pins
  Blynk.virtualWrite(V0, t);  // Temperature → Virtual Pin V0
  Blynk.virtualWrite(V1, h);  // Humidity → Virtual Pin V1
  Blynk.virtualWrite(V2, soilMoistureValue);  // Soil Moisture → Virtual Pin V2

  // Print sensor values to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C    Humidity: ");
  Serial.print(h);
  Serial.print(" %    Soil Moisture: ");
  Serial.println(soilMoistureValue);
}

void setup()
{
  // Start Serial communication
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, pass);
  int wifiAttempts = 0;

  // Try connecting for maximum 20 seconds
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
    delay(1000);
    Serial.print(".");
    wifiAttempts++;
  }

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  } else {
    Serial.println("Failed to connect to WiFi");
    return;  // Stop setup if WiFi fails
  }

  // Connect to Blynk cloud
  Blynk.begin(auth, ssid, pass);

  // Set timer to send sensor data every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop()
{
  // Run Blynk background tasks
  Blynk.run();

  // Run timer tasks
  timer.run();
}