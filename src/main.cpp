#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.hpp"
#include "HttpsClient.hpp"

String animationChars[] = {"/", "-", "\\", "|"};
int animationCounter = 0;

HttpsClient client(apiUri, apiKey);

void setup()
{
  // Initialize serial console.
  Serial.begin(115200);

  // Initialize random seed.
  randomSeed(analogRead(0));

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
  // Make HTTP request.
  String rand = String(random(30, 80));
  client.setClock();
  String payload;
  payload += "{\"data\":[{\"dataSourceId\":\"sensorbox0\",\"deviceId\": \"swipbox0\",\"values\": [{\"unit\": \"dB\",\"value\":";
  payload += rand;
  payload += "}]}]}";
  Serial.printf("%d POST /data\n", client.request("POST", "/data", payload));
  delay(1000);
}
