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

  const char* statusName(uint8_t st) {
    switch (st) {
      case WL_IDLE_STATUS:     return "IDLE";
      case WL_NO_SSID_AVAIL:   return "NO_SSID_AVAIL";
      case WL_CONNECTED:       return "CONNECTED";
      case WL_CONNECT_FAILED:  return "CONNECT_FAILED";
      case WL_CONNECTION_LOST: return "CONNECTION_LOST";
      case WL_DISCONNECTED:    return "DISCONNECTED";
      case WL_NO_SHIELD:       return "NO_SHIELD(no ESP on Serial1)";
      default:                 return "?";
    }
  }

  void ensureHardware() {
    if (hwInitialized) return;
    Serial1.setRX(ESP_RX_PIN);
    Serial1.setTX(ESP_TX_PIN);
    Serial1.begin(115200);
    delay(100);

    const bool ok = WiFi.init(&Serial1);
    Serial.print("[net] WiFi.init(Serial1)=");
    Serial.print(ok ? "ok" : "FAILED");
    Serial.print("  status=");
    Serial.println(statusName(WiFi.status()));

    // 只有握手成功才算初始化完成；失败留给下一次重试
    hwInitialized = ok;
  }
}

namespace NetworkService {

  void start() { }
  void poll()  { }

  bool connectOnce(unsigned long timeoutMs) {
    if (!loadWifiConfig()) {
      Serial.println("[net] /wifi.json missing or unparsable");
      return false;
    }
    if (WiFi.status() == WL_CONNECTED) return true;

    Serial.print("[net] ssid=\"");
    Serial.print(cachedSSID);
    Serial.println("\"");

    ensureHardware();

    if (WiFi.status() == WL_NO_SHIELD) {
      Serial.println("[net] abort: ESP8285 not responding on Serial1 (RX=GPIO1, TX=GPIO0)");
      return false;
    }

    char fw[32];
    Serial.print("[net] at-fw=");
    Serial.println(WiFi.firmwareVersion(fw));

    // 先扫描：判断 ESP 的射频是否真的能看到 AP。
    // 必须在 join 之前做——join 失败后 ESP 状态会变差（status 变成 NO_SHIELD），
    // 那时的扫描结果不可信。
    Serial.println("[net] scanning 2.4GHz...");
    const int8_t n = WiFi.scanNetworks();
    Serial.print("[net] scan found=");
    Serial.println(n);
    for (int8_t i = 0; i < n && i < 12; i++) {
      Serial.print("[net]   '");
      Serial.print(WiFi.SSID(i));
      Serial.print("'  ch=");
      Serial.print(WiFi.channel(i));
      Serial.print("  rssi=");
      Serial.println(WiFi.RSSI(i));
    }

    // 清理遗留状态
    WiFi.disconnect();
    delay(200);

    // WiFiEspAT 的 begin() 会阻塞到入网结果返回（实测约 26s），
    // 驱动内部有自己的 AT 超时，因此这个轮询预算已不再使用。
    (void)timeoutMs;
    const unsigned long t0 = millis();
    const int rc = WiFi.begin(cachedSSID.c_str(), cachedPassword.c_str());
    Serial.print("[net] begin rc=");
    Serial.print(statusName((uint8_t)rc));
    Serial.print(" after ");
    Serial.print((millis() - t0) / 1000);
    Serial.println("s");

    if (rc != WL_CONNECTED) {
      WiFi.disconnect();
      return false;
    }

    Serial.print("[net] connected, ip=");
    Serial.println(WiFi.localIP());
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