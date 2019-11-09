#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "HttpsClient.hpp"
#include "secrets.hpp"
#include "BlueDot_BME680.h"

const char deviceId[] = "swipbox0";

String animationChars[] = {"/", "-", "\\", "|"};
int animationCounter = 0;

HttpsClient client(apiUri, apiKey);
BlueDot_BME680 bme = BlueDot_BME680();

void setup()
{
  // Initialize serial console.
  Serial.begin(115200);

  // Start and configure I2C.
  Wire.begin();
  bme.parameter.I2CAddress = 0x76;
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
    Serial.println("Could not find BME");
    while (1)
    {
      // Do nothing.
    }
  }

  // Configure WiFi connection.
  WiFi.begin(ssid, pass);

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
  Serial.println(apiUri);
}

void loop()
{
  // Set clock and clear measurements.
  client.setClock();
  bme.writeCTRLMeas();

  // Send measurements.
  Serial.printf("%d POST /data > temp0\n", client.sendData(deviceId, "temp0", "°C", bme.readTempC()));
  Serial.printf("%d POST /data > hum0\n", client.sendData(deviceId, "hum0", "%", bme.readHumidity()));
  Serial.printf("%d POST /data > pres0\n", client.sendData(deviceId, "pres0", "hPa", bme.readPressure()));
  Serial.printf("%d POST /data > alt0\n", client.sendData(deviceId, "alt0", "m", bme.readAltitudeMeter()));
  Serial.printf("%d POST /data > gas0\n", client.sendData(deviceId, "gas0", "Ohms", bme.readGas()));

  // Don't kill my cellular data volume.
  delay(1000);
}
