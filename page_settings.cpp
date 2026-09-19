#include "page_settings.h"
#include "page_manager.h"
#include "settings.h"
#include "layout.h"

PageSettings settingsPage;

namespace {
  const char* items[PageSettings::ITEM_COUNT] = {
    "Rotation"
  };
}

void PageSettings::onEnter() {
  cursor = 0;
}

void PageSettings::onDraw(GraphicsAPI::Color accent) {
  auto& s = Settings::get();

  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Settings");

  // 唯一的设置项放在左列第一行
  int x0 = Layout::COL_X[0];
  int yc = Layout::ROW_Y[0];

  if (cursor == 0) {
    GraphicsAPI::drawRect(x0, yc - Layout::BOX_H / 2,
                          Layout::COL_W, Layout::BOX_H, accent);
  }

  GraphicsAPI::setCursor(x0 + Layout::TEXT_OFF_X,
                         yc + Layout::TEXT_OFF_Y);
  GraphicsAPI::print(items[0]);
  GraphicsAPI::print(": ");
  GraphicsAPI::print(s.rotation);

  // 右列第一行显示返回提示
  int hx = Layout::COL_X[1];
  int hy = Layout::ROW_Y[0];
  GraphicsAPI::setCursor(hx + Layout::TEXT_OFF_X,
                         hy + Layout::TEXT_OFF_Y);
  GraphicsAPI::print("Hold OK:");
  GraphicsAPI::setCursor(hx + Layout::TEXT_OFF_X,
                         hy + Layout::TEXT_OFF_Y + 12);
  GraphicsAPI::print("to Back");
}

void PageSettings::onInput(InputService::Button btn) {
  auto& s = Settings::get();

  if (btn == InputService::BTN_OK) {
    s.rotation = (s.rotation + 1) % 4;
    Settings::save();
    PageManager::draw();
  }
}

void PageSettings::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}