#include "app_weather.h"
#include "page_manager.h"
#include "network_service.h"
#include "layout.h"

#include <WiFiEspAT.h>
#include <ArduinoJson.h>

// ============ WMO 天气代码转文字 ============
static const char* weatherCodeToText(int code) {
  if (code == 0)                return "Clear";
  if (code >= 1 && code <= 3)   return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if (code >= 51 && code <= 57) return "Drizzle";
  if (code >= 61 && code <= 67) return "Rain";
  if (code >= 71 && code <= 77) return "Snow";
  if (code >= 80 && code <= 82) return "Showers";
  if (code >= 85 && code <= 86) return "Snow";
  if (code >= 95 && code <= 99) return "Storm";
  return "Unknown";
}

// ============ 天气页面 ============
class PageWeather : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Weather"; }

private:
  String temperature = "--";
  String description = "";
  String statusText  = "";
  bool   fetchWeather();
};

static PageWeather pageInstance;
static Page* getPage() { return &pageInstance; }
const App weatherApp = { "weather", "Weather", getPage };

// ============ 进入页面 ============
void PageWeather::onEnter() {
  if (!NetworkService::isConfigured()) {
    temperature = "--";
    description = "";
    statusText  = "No WiFi cfg";
    return;
  }

  if (!NetworkService::isConnected()) {
    temperature = "--";
    description = "";
    statusText  = "Offline";
    return;
  }

  statusText = "Fetching";
  fetchWeather();
}

// ============ 拉取天气 ============
bool PageWeather::fetchWeather() {
  if (!NetworkService::lockUart(5000)) {
    statusText = "Busy";
    return false;
  }

  bool result = false;
  do {
    // 黄岛区坐标：35.96°N, 120.19°E
    const char* host = "api.open-meteo.com";
    String path = "/v1/forecast"
                  "?latitude=35.96"
                  "&longitude=120.19"
                  "&current=temperature_2m,weather_code,relative_humidity_2m";

    WiFiClient client;
    if (!client.connect(host, 80)) {
      statusText = "Conn fail";
      break;
    }

    client.print(String("GET ") + path + " HTTP/1.0\r\n");
    client.print(String("Host: ") + host + "\r\n");
    client.print("User-Agent: curl/7.68.0\r\n");
    client.print("\r\n");

    // 等待首字节
    unsigned long t0 = millis();
    while (client.available() == 0) {
      if (millis() - t0 > 15000) {
        client.stop();
        statusText = "Timeout";
        break;
      }
      delay(10);
    }
    if (statusText == "Timeout") break;

    // 读取响应
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

    // 分离 body
    int bodyStart = response.indexOf("\r\n\r\n");
    if (bodyStart < 0) {
      statusText = "No body";
      break;
    }
    String body = response.substring(bodyStart + 4);

    // 解析 JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
      statusText = "JSON err";
      break;
    }

    float temp  = doc["current"]["temperature_2m"]     | 0.0f;
    int   wcode = doc["current"]["weather_code"]       | -1;
    int   humid = doc["current"]["relative_humidity_2m"] | 0;

    char buf[24];
    snprintf(buf, sizeof(buf), "%.1fC", temp);
    temperature = buf;

    snprintf(buf, sizeof(buf), "%s %d%%", weatherCodeToText(wcode), humid);
    description = buf;

    statusText = "OK";
    result = true;
  } while (0);

  NetworkService::unlockUart();
  return result;
}

// ============ 绘制 ============
void PageWeather::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Huangdao");

  int x = Layout::COL_X[0] + Layout::TEXT_OFF_X;

  // 温度（位置不变）
  GraphicsAPI::setCursor(x, 32);
  GraphicsAPI::print(temperature);

  // 天气描述
  if (description.length() > 0) {
    GraphicsAPI::setCursor(x, 48);
    GraphicsAPI::print(description);
  }

  // 状态（上移 20px：原 84 → 64）
  GraphicsAPI::setCursor(x, 64);
  GraphicsAPI::print("Stat: ");
  GraphicsAPI::print(statusText);

  GraphicsAPI::setCursor(Layout::TITLE_X, 90);
  GraphicsAPI::print("Hold OK: Back");
}

void PageWeather::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}