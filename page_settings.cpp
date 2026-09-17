#include "page_settings.h"
#include "page_manager.h"
#include "settings.h"

PageSettings settingsPage;

namespace {
  const char* items[PageSettings::ITEM_COUNT] = {
    "Color Mode",
    "Rotation",
    "Back"
  };
}

void PageSettings::onEnter() {
  cursor = 0;
}

void PageSettings::onDraw(GraphicsAPI::Color accent) {
  auto& s = Settings::get();

  GraphicsAPI::setCursor(20, 30);
  GraphicsAPI::print("Settings");

  for (int i = 0; i < ITEM_COUNT; i++) {
    int y = 70 + i * 30;
    GraphicsAPI::setCursor(40, y);

    if (i == 0) {
      GraphicsAPI::print(items[i]);
      GraphicsAPI::print(": ");
      GraphicsAPI::print(s.colorMode ? "BWR" : "BW");
    }
    else if (i == 1) {
      GraphicsAPI::print(items[i]);
      GraphicsAPI::print(": ");
      GraphicsAPI::print(s.rotation);
    }
    else {
      GraphicsAPI::print(items[i]);
    }

    if (i == cursor) {
      GraphicsAPI::drawRect(30, y - 14, 220, 24, accent);
    }
  }
}

void PageSettings::onInput(InputService::Button btn) {
  auto& s = Settings::get();
  bool changed = false;

  if (btn == InputService::BTN_UP) {
    cursor = (cursor - 1 + ITEM_COUNT) % ITEM_COUNT;
    PageManager::draw();
    return;
  }
  if (btn == InputService::BTN_DOWN) {
    cursor = (cursor + 1) % ITEM_COUNT;
    PageManager::draw();
    return;
  }

  if (btn == InputService::BTN_OK) {
    if (cursor == 0) {
      s.colorMode = !s.colorMode;
      changed = true;
    }
    else if (cursor == 1) {
      s.rotation = (s.rotation + 1) % 4;
      changed = true;
    }
    else {
      PageManager::pop();
      PageManager::draw();
      return;
    }
  }

  if (changed) {
    Settings::save();
    PageManager::draw();
  }
}