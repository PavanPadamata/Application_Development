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
    // Print IP address
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
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<html><head><title>Environmental Data</title></head><body>");
            client.println("<h1>Environmental Data</h1>");
            // Reading temperature and humidity
            float temperature = dht.readTemperature();
            float humidity = dht.readHumidity();
            client.print("Temperature: ");
            client.print(temperature);
            client.println(" °C<br>");
            client.print("Humidity: ");
            client.print(humidity);
            client.println(" %");
            client.println("<br>");
            // Reading soil moisture
        int soilMoisture = analogRead(ANALOG_PIN);
        client.print("Soil Moisture: ");
        client.println(soilMoisture);
        client.println("<br>");

        // Reading sunlight
        float lux = lightMeter.readLightLevel();
        client.print("Light: ");
        client.print(lux);
        client.println(" lx<br>");

        client.println("</body></html>");
        break;
      } else {
        currentLine = "";
      }
    } else if (c != '\r') {
      currentLine += c;
    }
  }
}
// Clear the header variable
currentLine = "";
// Close the connection
client.stop();
Serial.println("Client disconnected.");
  }
}
