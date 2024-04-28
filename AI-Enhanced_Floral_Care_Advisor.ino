#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>

#define DHTPIN 15
#define DHTTYPE DHT22
#define ANALOG_PIN 36

const char *ssid = "robo";
const char *password = "123456789";

const char *gptServer = "api.openai.com";
const char *gptToken = "YOUR_API_TOKEN"; // Replace with your actual Plus API token


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

  // Print data on serial monitor for debugging
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

  // Send data to ChatGPT API
  String advice = getAdviceFromGPT(temperature, humidity, soilMoisture, lux);

  // Print advice on serial monitor
  Serial.println("Advice: " + advice);

  client.println("<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Environmental Data</title></head><body><h1>Environmental Data</h1><div id='sensorData'><p>Temperature: " + String(temperature, 2) + " °C</p><p>Humidity: " + String(humidity, 2) + " %</p><p>Soil Moisture: " + String(soilMoisture) + "</p><p>Light: " + String(lux, 2) + " lx</p><p>Advice: " + advice + "</p></div><script>setTimeout(function() { location.reload(); }, 5000);</script></body></html>");
}

String getAdviceFromGPT(float temperature, float humidity, int soilMoisture, float lux) {
  // Construct data string
  String data = "Temperature: " + String(temperature, 2) + " °C\n";
  data += "Humidity: " + String(humidity, 2) + " %\n";
  data += "Soil Moisture: " + String(soilMoisture) + "\n";
  data += "Light: " + String(lux, 2) + " lx\n";

  // Construct JSON request
  String jsonRequest = "{\"prompt\": \"" + data + "\", \"model\": \"text-davinci-003\", \"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + ", \"soil_moisture\": " + String(soilMoisture) + ", \"lux\": " + String(lux) + ", \"max_tokens\": 100}";

  HTTPClient http;
  http.begin("https://" + String(gptServer) + "/v1/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(gptToken));

  int httpResponseCode = http.POST(jsonRequest);
  String response = "";
  if (httpResponseCode > 0) {
    response = http.getString();
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();

  return response;
}
