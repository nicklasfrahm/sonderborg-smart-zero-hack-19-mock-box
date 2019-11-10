#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
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

  // Configure I/O.
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOOR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

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
}

void loop()
{
  // Open door.
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(DOOR_PIN, HIGH);

  // Set clock and clear measurements.
  client.setClock();
  bme.writeCTRLMeas();

  // Read light sensor.
  float lightIntensity = (float)analogRead(LIGHT_SENSOR_PIN) * 100.0 / 1024.0;

  // Send measurements.
  Serial.printf("%d POST /data > gas0\n", client.sendData(DEVICE_ID, "gas0", "Ohms", bme.readGas()));
  Serial.printf("%d POST /data > temp0\n", client.sendData(DEVICE_ID, "temp0", "°C", bme.readTempC()));
  Serial.printf("%d POST /data > hum0\n", client.sendData(DEVICE_ID, "hum0", "%", bme.readHumidity()));
  Serial.printf("%d POST /data > pres0\n", client.sendData(DEVICE_ID, "pres0", "hPa", bme.readPressure()));
  Serial.printf("%d POST /data > alt0\n", client.sendData(DEVICE_ID, "alt0", "m", bme.readAltitudeMeter()));
  Serial.printf("%d POST /data > light0\n", client.sendData(DEVICE_ID, "light0", "%", lightIntensity));

  // Lock door.
  digitalWrite(LED_PIN, LOW);
  digitalWrite(DOOR_PIN, LOW);

  ESP.deepSleep(0);
}
