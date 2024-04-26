#include <DHT.h>
#include <Wire.h>
#include <BH1750.h> // Include the BH1750 library

#define DHTPIN 15       // Pin where the DHT22 is connected
#define DHTTYPE DHT22  // Type of DHT sensor
#define ANALOG_PIN 36  // Analog pin for soil moisture sensor

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter; // I2C address 0x23

void setup() {
  Serial.begin(9600);
  dht.begin();
  Wire.begin();
  lightMeter.begin(); // Initialize the light sensor
  Serial.println(F("BH1750 Test begin"));
}

void loop() {
  delay(2000);  // Wait for 2 seconds between measurements

  // Reading temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Reading soil moisture
  int soilMoisture = analogRead(ANALOG_PIN);

  // Reading sunlight
  float lux = lightMeter.readLightLevel();

  // Print the sensor readings
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);

  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  Serial.println("-------------------------");
}
