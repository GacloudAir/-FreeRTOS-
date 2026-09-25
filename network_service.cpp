#include "network_service.h"
#include "storage_service.h"
#include <WiFiEspAT.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>
#include <semphr.h>

namespace {
  #define ESP_RX_PIN  1
  #define ESP_TX_PIN  0

  const char* WIFI_CONFIG_PATH = "/wifi.json";

  enum State {
    ST_IDLE,
    ST_UNCONFIGURED,     // 无 /wifi.json
    ST_CONNECTING,
    ST_CONNECTED,
    ST_RETRY_WAIT
  };

  State state = ST_IDLE;
  unsigned long stateEnterMs = 0;

  const unsigned long CONNECT_TIMEOUT_MS = 10000;
  const unsigned long RETRY_DELAY_MS     = 5000;

  SemaphoreHandle_t uartMutex = nullptr;

  String cachedSSID;
  String cachedPassword;
  bool   configLoaded = false;

  bool loadWifiConfig() {
    if (!StorageService::exists(WIFI_CONFIG_PATH)) {
      return false;
    }
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
}

namespace NetworkService {

  bool lockUart(unsigned long timeoutMs) {
    if (!uartMutex) {
      uartMutex = xSemaphoreCreateMutex();
      if (!uartMutex) return false;
    }
    if (timeoutMs == 0) {
      return xSemaphoreTake(uartMutex, 0) == pdTRUE;
    }
    return xSemaphoreTake(uartMutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE;
  }

  void unlockUart() {
    if (uartMutex) xSemaphoreGive(uartMutex);
  }

  void start() {
    // 空实现
  }

  void poll() {
    if (!lockUart(0)) return;

    unsigned long now = millis();

    // ============ 首次初始化 ============
    if (state == ST_IDLE) {
      // 尝试读取配置
      if (!loadWifiConfig()) {
        state = ST_UNCONFIGURED;
        configLoaded = false;
        unlockUart();
        return;
      }

      // 有配置，初始化 ESP8285
      Serial1.setRX(ESP_RX_PIN);
      Serial1.setTX(ESP_TX_PIN);
      Serial1.begin(115200);
      delay(100);

      WiFi.init(&Serial1);
      WiFi.disconnect();
      delay(1000);
      WiFi.begin(cachedSSID.c_str(), cachedPassword.c_str());

      state = ST_CONNECTING;
      stateEnterMs = now;
      configLoaded = true;
      unlockUart();
      return;
    }

    // ============ 状态推进 ============
    switch (state) {
      case ST_UNCONFIGURED:
        // 不做任何事，等待外部写入 /wifi.json 并重启
        break;

      case ST_CONNECTING:
        if (WiFi.status() == WL_CONNECTED) {
          state = ST_CONNECTED;
          stateEnterMs = now;
        } else if (now - stateEnterMs >= CONNECT_TIMEOUT_MS) {
          state = ST_RETRY_WAIT;
          stateEnterMs = now;
        }
        break;

      case ST_CONNECTED:
        if (WiFi.status() != WL_CONNECTED) {
          state = ST_RETRY_WAIT;
          stateEnterMs = now;
        }
        break;

      case ST_RETRY_WAIT:
        if (now - stateEnterMs >= RETRY_DELAY_MS) {
          WiFi.begin(cachedSSID.c_str(), cachedPassword.c_str());
          state = ST_CONNECTING;
          stateEnterMs = now;
        }
        break;

      default:
        break;
    }

    unlockUart();
  }

  bool isConnected() {
    return state == ST_CONNECTED && WiFi.status() == WL_CONNECTED;
  }

  bool isConfigured() {
    return configLoaded;
  }

  String getTimeString() {
    unsigned long s = millis() / 1000;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu", s / 60, s % 60);
    return String(buf);
  }
}