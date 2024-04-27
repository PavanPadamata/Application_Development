#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define ANALOG_PIN 36

const char *ssid = "robo";
const char *password = "123456789";

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println();
  Serial.print("Connected to Wi-Fi network with IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");

  dht.begin();
  Wire.begin();
  lightMeter.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            handleClientRequest(client);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}

void handleClientRequest(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type: text/html");
  client.println();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(ANALOG_PIN);
  float lux = lightMeter.readLightLevel();

  client.println("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Environmental Data</title></head><body><h1>Environmental Data</h1><div id='sensorData'><p>Temperature: " + String(temperature, 2) + " °C</p><p>Humidity: " + String(humidity, 2) + " %</p><p>Soil Moisture: " + String(soilMoisture) + "</p><p>Light: " + String(lux, 2) + " lx</p></div><script>setTimeout(function() { location.reload(); }, 5000);</script></body></html>");
}
