#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "secrets.hpp"

String animationChars[] = {"/", "-", "\\", "|"};
int animationCounter = 0;

void setup()
{
  // Initialize serial console.
  Serial.begin(115200);

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

  // Print idle state.
  Serial.print("Idle: ");
}

void loop()
{
  // Display minor animation after successful connection.
  Serial.printf("%s", animationChars[animationCounter].c_str());
  animationCounter = (animationCounter + 1) % 4;
  delay(100);
  Serial.print("\b");
}
