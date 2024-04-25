#include <DHT.h>                // Include DHT library
#include <Wire.h>               // Include Wire library for I2C
#include <BH1750FVI.h>          // Include BH1750FVI library for light sensor
#include <ESP8266WiFi.h>        // Include ESP8266WiFi library

#define DHTPIN D2               // Define the pin for DHT11 sensor
#define DHTTYPE DHT11           // Define the type of DHT sensor

#define SOIL_MOISTURE_PIN A0    // Define the pin for soil moisture sensor
#define LIGHT_SENSOR_ADDR 0x23  // I2C address for BH1750FVI light sensor

DHT dht(DHTPIN, DHTTYPE);       // Initialize DHT sensor
BH1750FVI lightMeter;

const char* ssid = "YOUR_WIFI_SSID";      // Your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD";  // Your WiFi password

void setup() {
  Serial.begin(9600);
  dht.begin();
  lightMeter.begin(BH1750FVI::ONE_TIME_HIGH_RES_MODE, LIGHT_SENSOR_ADDR);
  
  connectToWiFi();
}

void loop() {
  // Read temperature and humidity from DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read soil moisture
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  // Read light intensity
  uint16_t lightIntensity = lightMeter.readLightLevel();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);

  Serial.print("Light Intensity: ");
  Serial.println(lightIntensity);

  delay(2000);  // Delay for 2 seconds
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}
