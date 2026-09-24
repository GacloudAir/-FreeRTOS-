#include "page_settings.h"
#include "page_manager.h"
#include "settings.h"
#include "layout.h"

PageSettings settingsPage;

namespace {
  const char* items[PageSettings::ITEM_COUNT] = {
    "Rotation",
    "Refresh"
  };
}

void PageSettings::onEnter() {
  cursor = 0;
}

void PageSettings::onDraw(GraphicsAPI::Color accent) {
  auto& s = Settings::get();

  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Settings");

  for (int i = 0; i < ITEM_COUNT; i++) {
    int col = i % 2;
    int row = i / 2;
    int x0 = Layout::COL_X[col] + Layout::SETTINGS_X_SHIFT;   // ← 加偏移
    int yc = Layout::ROW_Y[row];

    if (i == cursor) {
      GraphicsAPI::drawRect(x0, yc - Layout::BOX_H / 2,
                            Layout::COL_W, Layout::BOX_H, accent);
    }

    GraphicsAPI::setCursor(x0 + Layout::TEXT_OFF_X,
                           yc + Layout::TEXT_OFF_Y);
    GraphicsAPI::print(items[i]);
    GraphicsAPI::print(":");

    if (i == 0) {
      GraphicsAPI::print(s.rotation);
    } else {
      GraphicsAPI::print(s.refreshMode == Settings::REFRESH_SMART
                         ? "Smart" : "Block");
    }
  }

  GraphicsAPI::setCursor(Layout::TITLE_X, 90);
  GraphicsAPI::print("Hold OK: Back");
}

void PageSettings::onInput(InputService::Button btn) {
  auto& s = Settings::get();

  if (btn == InputService::BTN_UP) {
    if (cursor > 0) { cursor--; PageManager::markDirty(); }
    return;
  }
  if (btn == InputService::BTN_DOWN) {
    if (cursor + 1 < ITEM_COUNT) { cursor++; PageManager::markDirty(); }
    return;
  }
  if (btn == InputService::BTN_OK) {
    if (cursor == 0) {
      s.rotation = (s.rotation + 1) % 4;
    } else if (cursor == 1) {
      s.refreshMode = (s.refreshMode + 1) % 2;
    }
    Settings::save();
    PageManager::markDirty();
  }
}

// ← 关键：这个定义不能漏
void PageSettings::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}