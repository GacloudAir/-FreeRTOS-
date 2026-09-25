#include "app_clock.h"
#include "page_manager.h"
#include "time_service.h"
#include "layout.h"
#include <Fonts/FreeSansBold24pt7b.h>

class PageClock : public Page {
public:
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Clock"; }
};

static PageClock pageInstance;
static Page* getPage() { return &pageInstance; }
const App clockApp = { "clock", "Clock", getPage };

void PageClock::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Clock");

  if (!TimeService::isSynced()) {
    GraphicsAPI::setCursor(Layout::COL_X[0] + Layout::TEXT_OFF_X, 50);
    GraphicsAPI::print("Syncing...");
    GraphicsAPI::setCursor(Layout::COL_X[0] + Layout::TEXT_OFF_X, 70);
    GraphicsAPI::print("Press UP to retry");
    return;
  }

  // 大字时间
  GraphicsAPI::setFont(&FreeSansBold24pt7b);
  GraphicsAPI::setCursor(20, 62);
  GraphicsAPI::print(TimeService::formatTime());

  // 日期 + 星期
  GraphicsAPI::setFont(NULL);
  GraphicsAPI::setCursor(22, 78);
  GraphicsAPI::print(TimeService::formatDate());
  GraphicsAPI::print("  ");
  GraphicsAPI::print(TimeService::formatWeekday());

  GraphicsAPI::setCursor(Layout::TITLE_X, 92);
  GraphicsAPI::print("UP: Sync  Hold OK: Back");
}

void PageClock::onInput(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    // 手动强制同步
    TimeService::start();
    PageManager::markDirty();
  }
}

void PageClock::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}