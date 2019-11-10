#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <math.h>
#include "HttpsClient.hpp"
#include "secrets.hpp"
#include "BlueDot_BME680.h"

const char DEVICE_ID[] = "swipbox0";
const int LIGHT_SENSOR_PIN = A0;
const int BUTTON_PIN = 14; // D5
const int DOOR_PIN = 12;   // D6
const int LED_PIN = 13;    // D7

String animationChars[] = {"/", "-", "\\", "|"};
int animationCounter = 0;

HttpsClient client(API_URI, API_KEY);
BlueDot_BME680 bme = BlueDot_BME680();

void setup()
{
  // Initialize serial console.
  Serial.begin(115200);

  // Configure I/O.
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Open door and enable LED.
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(DOOR_PIN, HIGH);

  // Start and configure I2C.
  Wire.begin();
  bme.parameter.I2CAddress = 0x77;
  bme.parameter.sensorMode = 0b01;
  bme.parameter.IIRfilter = 0b100;
  bme.parameter.humidOversampling = 0b101;
  bme.parameter.tempOversampling = 0b101;
  bme.parameter.pressOversampling = 0b101;
  bme.parameter.pressureSeaLevel = 1013.25;
  bme.parameter.tempOutsideCelsius = 4;
  bme.parameter.target_temp = 320;

  if (bme.init() != 0x61)
  {
    Serial.println("BME: Failure");
    while (1)
    {
      // Do nothing.
    }
  }

  // Configure WiFi connection.
  WiFi.begin(SSID, PASS);

  Serial.print("WiFi connection: ");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(animationChars[animationCounter].c_str());
    animationCounter = (animationCounter + 1) % 4;
    delay(100);
    Serial.print("\b");
  }
  Serial.println("\rWiFi connection: Success");

  // Print basic info.
  Serial.print("API URI: ");
  Serial.println(API_URI);

  // Set clock and clear measurements.
  client.setClock();
  bme.writeCTRLMeas();

  // Send measurements.
  int statusCode = 0;
  float datapoint = 0;

  datapoint = bme.readGas();
  statusCode = client.sendData(DEVICE_ID, "gas0", "Ohms", datapoint);
  Serial.printf("%d POST /data > gas0   > %.2f\n", statusCode, datapoint);

  datapoint = bme.readTempC();
  statusCode = client.sendData(DEVICE_ID, "temp0", "°C", datapoint);
  Serial.printf("%d POST /data > temp0  > %.2f\n", statusCode, datapoint);

  datapoint = bme.readHumidity();
  statusCode = client.sendData(DEVICE_ID, "hum0", "%", datapoint);
  Serial.printf("%d POST /data > hum0   > %.2f\n", statusCode, datapoint);

  datapoint = bme.readPressure();
  statusCode = client.sendData(DEVICE_ID, "pres0", "hPa", datapoint);
  Serial.printf("%d POST /data > pres0  > %.2f\n", statusCode, datapoint);

  datapoint = bme.readAltitudeMeter();
  statusCode = client.sendData(DEVICE_ID, "alt0", "m", datapoint);
  Serial.printf("%d POST /data > alt0   > %.2f\n", statusCode, datapoint);

  datapoint = (float)map(analogRead(LIGHT_SENSOR_PIN), 0, 1024, 0, 100);
  statusCode = client.sendData(DEVICE_ID, "light0", "%", datapoint);
  Serial.printf("%d POST /data > light0 > %.2f\n", statusCode, datapoint);

  // Lock door.
  digitalWrite(LED_PIN, LOW);
  digitalWrite(DOOR_PIN, LOW);

  ESP.deepSleep(0);
}

void loop() {}
