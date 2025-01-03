#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_AS7331.h>
#include <string>

#define SOIL_SENSOR 25

// Wi-Fi credentials
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

Adafruit_AHTX0 htSensor;
SfeAS7331ArdI2C uvSensor;

void setup() {
  delay(3000);
  Serial.begin(9600);

  // Wi-Fi Setup
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status()) != WL_CONNECTED {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP address: ");
  Serial.println(WiFi.localIP());

  // Soil Moisture Sensor Setup
  pinMode(SOIL_SENSOR, OUTPUT);

  // Humidity and Temperature Sensor Setup
  if (! htSensor.begin()) {
    Serial.println("DHT20 Not Found");
    while (1) delay (10);
  }
  Serial.println("DHT20 Found!");

  // UV Sensor Setup
  Wire.begin();
  if (!uvSensor.begin()) {
    Serial.println("UV Sensor Not Found");
    while (1) delay (10);
  }
  Serial.println("UV Sensor Found!");

  if (!uvSensor.prepareMeasurement(MEAS_MODE_CMD)) {
    Serial.println("Sensor mode not set properly");
    while (1) delay (10);
  }
}

void loop() {
  // Retrieve data from sensors
  int soilMoisture = analogRead(SOIL_SENSOR);

  sensors_event_t humidityEvent, tempEvent;
  htSensor.getEvent(&humidityEvent, &tempEvent);
  float humidity = humidityEvent.relative_humidity;
  float temp = tempEvent.temperature;

  if (kSTkErrOk != uvSensor.setStartState(true)) {
    Serial.println("Error reading from UV Sensor");
  }
  delay(5 + uvSensor.getConversionTimeMillis());
  float uva = uvSensor.getUVA();
  float uvb = uvSensor.getUVB();

  // Checks WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

  }

  // Output data to serial monitor
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  Serial.print("Temperature: ");
  Serial.println(temp, 3);
  Serial.print("Humidity: ");
  Serial.println(humidity, 3);
  Serial.print("UVA Rays: ");
  Serial.println(uva, 3);
  Serial.print("UVB Rays: ");
  Serial.println(uvb, 3);
  Serial.println("\n");

  delay(1000);
}
