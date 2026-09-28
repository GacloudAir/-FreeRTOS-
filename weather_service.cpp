#include "weather_service.h"
#include "network_service.h"
#include <WiFiEspAT.h>
#include <ArduinoJson.h>

namespace {
  WeatherService::Data current = { 0.0f, -1, 0, false, 0 };
}

namespace WeatherService {

  const Data& get() { return current; }

  bool fetch() {
    if (!NetworkService::isConnected()) return false;

    const char* host = "api.open-meteo.com";
    String path = "/v1/forecast"
                  "?latitude=35.96"
                  "&longitude=120.19"
                  "&current=temperature_2m,weather_code,relative_humidity_2m";

    WiFiClient client;
    if (!client.connect(host, 80)) return false;

    client.print(String("GET ") + path + " HTTP/1.0\r\n");
    client.print(String("Host: ") + host + "\r\n");
    client.print("User-Agent: curl/7.68.0\r\n\r\n");

    unsigned long t0 = millis();
    while (client.available() == 0) {
      if (millis() - t0 > 15000) { client.stop(); return false; }
      delay(10);
    }

    String response = "";
    t0 = millis();
    while (client.connected() || client.available()) {
      if (client.available()) {
        response += (char)client.read();
        t0 = millis();
      } else if (millis() - t0 > 3000) {
        break;
      }
    }
    client.stop();

    int bodyStart = response.indexOf("\r\n\r\n");
    if (bodyStart < 0) return false;
    String body = response.substring(bodyStart + 4);

    JsonDocument doc;
    if (deserializeJson(doc, body)) return false;

    current.temp      = doc["current"]["temperature_2m"]       | 0.0f;
    current.wcode     = doc["current"]["weather_code"]         | -1;
    current.humid     = doc["current"]["relative_humidity_2m"] | 0;
    current.valid     = true;
    current.timestamp = millis();
    return true;
  }

  bool isFresh(unsigned long maxAgeMs) {
    if (!current.valid) return false;
    return (millis() - current.timestamp) < maxAgeMs;
  }
}