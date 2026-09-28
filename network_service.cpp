#include "network_service.h"
#include "storage_service.h"
#include <WiFiEspAT.h>
#include <ArduinoJson.h>

namespace {
  #define ESP_RX_PIN  1
  #define ESP_TX_PIN  0

  const char* WIFI_CONFIG_PATH = "/wifi.json";

  bool hwInitialized = false;
  String cachedSSID;
  String cachedPassword;

  bool loadWifiConfig() {
    cachedSSID = "";
    cachedPassword = "";

    if (!StorageService::exists(WIFI_CONFIG_PATH)) return false;
    String json = StorageService::read(WIFI_CONFIG_PATH);
    if (json.length() == 0) return false;

    JsonDocument doc;
    if (deserializeJson(doc, json)) return false;

    const char* s = doc["ssid"]     | "";
    const char* p = doc["password"] | "";
    if (strlen(s) == 0) return false;

    cachedSSID     = String(s);
    cachedPassword = String(p);
    return true;
  }

  void ensureHardware() {
    if (hwInitialized) return;
    Serial1.setRX(ESP_RX_PIN);
    Serial1.setTX(ESP_TX_PIN);
    Serial1.begin(115200);
    delay(100);
    WiFi.init(&Serial1);
    hwInitialized = true;
  }
}

namespace NetworkService {

  void start() { }
  void poll()  { }

  bool connectOnce(unsigned long timeoutMs) {
    if (!loadWifiConfig()) return false;
    if (WiFi.status() == WL_CONNECTED) return true;

    ensureHardware();

    // 清理遗留状态
    WiFi.disconnect();
    delay(200);

    WiFi.begin(cachedSSID.c_str(), cachedPassword.c_str());

    unsigned long t0 = millis();
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - t0 > timeoutMs) {
        WiFi.disconnect();
        return false;
      }
      delay(200);
    }
    return true;
  }

  void disconnect() {
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
      delay(100);
    }
  }

  bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
  }

  bool isConfigured() {
    return StorageService::exists(WIFI_CONFIG_PATH);
  }

  String getTimeString() {
    unsigned long s = millis() / 1000;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu", s / 60, s % 60);
    return String(buf);
  }
}