#include "app_main_page.h"
#include "page_manager.h"
#include "page_main_menu.h"
#include "time_service.h"
#include "network_service.h"
#include "graphics_API.h"
#include "layout.h"
#include "weather_icons.h"
#include "kernel_tasks.h"
#include <WiFiEspAT.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <Fonts/FreeSansBold18pt7b.h>

class PageMainPage : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onTick() override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "MainPage"; }

private:
  float temp = 0.0f;
  int   wcode = -1;
  int   humid = 0;
  bool  hasWeather = false;
  unsigned long lastWeatherSync = 0;
  unsigned long lastClockRefresh = 0;

  bool fetchWeather();
};

static PageMainPage pageInstance;
static Page* getPage() { return &pageInstance; }
const App mainPageApp = { "home", "Home", getPage };

// ============ 进入页面 ============
void PageMainPage::onEnter() {
  lastClockRefresh = millis();

  // 若数据过旧，立即同步
  if (NetworkService::isConnected() &&
      (!hasWeather || millis() - lastWeatherSync > 55UL * 60 * 1000)) {
    if (fetchWeather()) {
      lastWeatherSync = millis();
    }
  }
}

// ============ 周期性检查（由 taskBg 每秒调用一次） ============
void PageMainPage::onTick() {
  unsigned long now = millis();

  // 天气同步：超过 55 分钟
  if (NetworkService::isConnected() &&
      (!hasWeather || now - lastWeatherSync > 55UL * 60 * 1000)) {
    if (fetchWeather()) {
      lastWeatherSync = now;
      PageManager::markDirty();
      KernelTasks::requestRefresh();
    }
  }

  // 时钟刷新：每 5 分钟
  if (now - lastClockRefresh > 5UL * 60 * 1000) {
    lastClockRefresh = now;
    PageManager::markDirty();
    KernelTasks::requestRefresh();
  }
}

// ============ 拉取天气 ============
bool PageMainPage::fetchWeather() {
  if (!NetworkService::lockUart(5000)) return false;

  bool result = false;
  do {
    const char* host = "api.open-meteo.com";
    String path = "/v1/forecast"
                  "?latitude=35.96"
                  "&longitude=120.19"
                  "&current=temperature_2m,weather_code,relative_humidity_2m";

    WiFiClient client;
    if (!client.connect(host, 80)) break;

    client.print(String("GET ") + path + " HTTP/1.0\r\n");
    client.print(String("Host: ") + host + "\r\n");
    client.print("User-Agent: curl/7.68.0\r\n\r\n");

    unsigned long t0 = millis();
    while (client.available() == 0) {
      if (millis() - t0 > 15000) { client.stop(); break; }
      delay(10);
    }
    if (!client.connected() && !client.available()) break;

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
    if (bodyStart < 0) break;
    String body = response.substring(bodyStart + 4);

    JsonDocument doc;
    if (deserializeJson(doc, body)) break;

    temp  = doc["current"]["temperature_2m"]        | 0.0f;
    wcode = doc["current"]["weather_code"]          | -1;
    humid = doc["current"]["relative_humidity_2m"]  | 0;
    hasWeather = true;
    result = true;
  } while (0);

  NetworkService::unlockUart();
  return result;
}

// ============ 绘制 ============
void PageMainPage::onDraw(GraphicsAPI::Color accent) {
  // 顶部：天气图标（左上角）
  if (hasWeather) {
    GraphicsAPI::drawBitmap(4, 4, weatherIconFor(wcode), 16, 16, GraphicsAPI::BLACK);

    char buf[32];
    snprintf(buf, sizeof(buf), "%.1fC  %s  %d%%",
             temp, weatherTextFor(wcode), humid);
    GraphicsAPI::setCursor(26, 12);
    GraphicsAPI::print(buf);
  } else {
    GraphicsAPI::setCursor(6, 12);
    GraphicsAPI::print("Waiting weather...");
  }

  // 中间：时间大字
  GraphicsAPI::setFont(&FreeSansBold18pt7b);
  GraphicsAPI::setCursor(24, 64);
  GraphicsAPI::print(TimeService::formatTime());

  // 日期 + 星期
  GraphicsAPI::setFont(NULL);
  GraphicsAPI::setCursor(24, 82);
  GraphicsAPI::print(TimeService::formatDate());
  GraphicsAPI::print("  ");
  GraphicsAPI::print(TimeService::formatWeekday());

  // 底部提示
  GraphicsAPI::setCursor(6, 96);
  GraphicsAPI::print("Hold OK: Menu");
}

// ============ 长按 OK 进入主菜单 ============
void PageMainPage::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::push(&mainMenu);
  }
}