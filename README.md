# Sonderborg Smart Zero Hack 2019 - Mock Box

This project is a small application that runs on an ESP8266. It connects with the open data portal of the city and sends telemetric data. In the future this functionality will be performed by the nærboks.

## Development

You will need a PlatformIO IDE. Create the file `src/secrets.hpp` with the following contents:

```cpp
// WiFi network connection details.
const char ssid[] = "MY_SSID";
const char pass[] = "MY_PASS";

// API connection details.
const char apiUri[] = "MY_API_URI";
const char apiKey[] = "MY_API_KEY";
```

## Important information

Make sure that the NodeMCU is not too far from the WiFi access point as this might lead to unexpected error codes, such as `-11`.
