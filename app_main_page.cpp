#include "app_main_page.h"
#include "page_manager.h"
#include "page_main_menu.h"
#include "time_service.h"
#include "weather_service.h"
#include "network_service.h"
#include "graphics_API.h"
#include "layout.h"
#include "weather_icons.h"
#include "kernel_tasks.h"

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
  unsigned long lastClockRefresh = 0;
};

static PageMainPage pageInstance;
static Page* getPage() { return &pageInstance; }
const App mainPageApp = { "home", "Home", getPage };

void PageMainPage::onEnter() {
  lastClockRefresh = millis();
}

void PageMainPage::onTick() {
  unsigned long now = millis();
  if (now - lastClockRefresh > 5UL * 60 * 1000) {
    lastClockRefresh = now;
    PageManager::markDirty();
    KernelTasks::requestRefresh();
  }
}

void PageMainPage::onDraw(GraphicsAPI::Color accent) {
  const auto& w = WeatherService::get();

  // 左上角：天气图标 + 摘要
  if (w.valid) {
    GraphicsAPI::drawBitmap(4, 4, weatherIconFor(w.wcode), 16, 16, GraphicsAPI::BLACK);

    char buf[32];
    snprintf(buf, sizeof(buf), "%.1fC  %s  %d%%",
             w.temp, weatherTextFor(w.wcode), w.humid);
    GraphicsAPI::setCursor(26, 12);
    GraphicsAPI::print(buf);
  } else {
    GraphicsAPI::setCursor(6, 12);
    GraphicsAPI::print("Waiting weather...");
  }

  // 中间：大字时间
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

void PageMainPage::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::push(&mainMenu);
  }
}