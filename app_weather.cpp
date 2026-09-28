#include "app_weather.h"
#include "page_manager.h"
#include "time_service.h"
#include "weather_service.h"
#include "graphics_API.h"
#include "layout.h"
#include "weather_icons.h"
#include "kernel_tasks.h"

#include <stdio.h>

class PageWeather : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Weather"; }

private:
  unsigned long lastRefresh = 0;
};

static PageWeather pageInstance;
static Page* getPage() { return &pageInstance; }
const App weatherApp = { "weather", "Weather", getPage };

void PageWeather::onEnter() {
  // 进入页面时，如果数据过期，请求后台同步
  if (!WeatherService::isFresh(55UL * 60 * 1000)) {
    KernelTasks::requestRefresh();   // 唤醒 taskBg（可选）
  }
}

void PageWeather::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Huangdao");

  const auto& w = WeatherService::get();
  int x = Layout::COL_X[0] + Layout::TEXT_OFF_X;

  if (!w.valid) {
    GraphicsAPI::setCursor(x, 50);
    GraphicsAPI::print("No data");
    GraphicsAPI::setCursor(x, 70);
    GraphicsAPI::print("Wait for sync...");
  } else {
    // 图标
    GraphicsAPI::drawBitmap(4, 24, weatherIconFor(w.wcode), 16, 16, GraphicsAPI::BLACK);

    char buf[32];

    GraphicsAPI::setCursor(x + 20, 32);
    snprintf(buf, sizeof(buf), "%.1fC", w.temp);
    GraphicsAPI::print(buf);

    GraphicsAPI::setCursor(x + 20, 48);
    GraphicsAPI::print(weatherTextFor(w.wcode));

    GraphicsAPI::setCursor(x, 64);
    snprintf(buf, sizeof(buf), "Humidity: %d%%", w.humid);
    GraphicsAPI::print(buf);

    // 数据时间
    unsigned long age = (millis() - w.timestamp) / 60000;
    GraphicsAPI::setCursor(x, 80);
    snprintf(buf, sizeof(buf), "Updated: %lum ago", age);
    GraphicsAPI::print(buf);
  }

  GraphicsAPI::setCursor(Layout::TITLE_X, 92);
  GraphicsAPI::print("Hold OK: Back");
}

void PageWeather::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}