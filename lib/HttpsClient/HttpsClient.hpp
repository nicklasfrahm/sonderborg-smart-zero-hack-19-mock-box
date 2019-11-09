#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiServerSecureBearSSL.h>

class HttpsClient
{
public:
  HttpsClient(const char apiUri[], const char apiKey[]);
  ~HttpsClient(void);
  time_t setClock(void);
  int request(const char method[], const char endpoint[], String &payload);
  int sendData(const char deviceId[], const char dataSourceId[], const char unit[], float value);

private:
  String apiUri;
  String apiKey;
  BearSSL::X509List *certList;
  BearSSL::WiFiClientSecure *client;
  BearSSL::Session *session;
  HTTPClient *https;
};
