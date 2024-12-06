#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_AS7331.h>
#include <string>
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#define SOIL_SENSOR 32

Adafruit_AHTX0 htSensor;
SfeAS7331ArdI2C uvSensor;

char* ssid;    // network SSID
char* pass;    // network password

const char kHostname[] = "54.183.202.166";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;


// NVS access method from Lab 4 to retrieve SSID and Password
void nvs_access() {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || 
    err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK) {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    size_t required_size;
    nvs_get_str(my_handle, "ssid", NULL, &ssid_len);
    ssid = (char*)malloc(ssid_len);
    nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    nvs_get_str(my_handle, "pass", NULL, &pass_len);
    pass = (char*)malloc(pass_len);
    nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err) {
      case ESP_OK:
        Serial.printf("Done\n");
        Serial.printf("SSID = %s\n", ssid);
        Serial.printf("PASSWD = %s\n", pass);
        break;
      case ESP_ERR_NVS_NOT_FOUND:
        Serial.printf("The value is not initialized yet!\n");
        break;
      default:
        Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}

void setup() {
  Serial.begin(9600);
  delay(5000);

  // Retreive SSID and password
  nvs_access();

  // Wi-Fi Setup
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
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
  // Setup WiFi and Http Client
  WiFiClient c;
  HttpClient http(c);

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

  // Send data to server
  int err = 0;

  // Setup Query
  std::string queryString = {"/?soil=" + std::to_string(soilMoisture) + "&temp=" + std::to_string(temp) + "&hum=" + 
    std::to_string(humidity) + "&uva=" + std::to_string(uva) + "&uvb=r" + std::to_string(uvb)};
  int n = queryString.length();
  char q[n+1];
  strcpy(q, queryString.c_str());


  err = http.get(kHostname, 5000, q, NULL);
  if (err == 0) {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err); 
      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ((http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout)) {
          if (http.available()) {
            c = http.read();
            // Print out this character
            Serial.print(c);
            bodyLen--;
            // We read something, reset the timeout counter
            timeoutStart = millis();
          } else {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(kNetworkDelay);
          }
        }
      } else {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    } else {
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  } else {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }

  http.stop();
  while (1);
}
