#include "page_main_menu.h"
#include "page_manager.h"
#include "page_settings.h"
#include "settings.h"

namespace {
  const char* items[PageMainMenu::ITEM_COUNT] = {
    "Settings",
    "About",
    "Exit"
  };
}

void PageMainMenu::onEnter() {
  cursor = 0;
}

void PageMainMenu::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(20, 30);
  GraphicsAPI::print("Main Menu");

  for (int i = 0; i < ITEM_COUNT; i++) {
    int y = 70 + i * 30;
    GraphicsAPI::setCursor(40, y);
    GraphicsAPI::print(items[i]);
    if (i == cursor) {
      GraphicsAPI::drawRect(30, y - 14, 200, 24, accent);
    }
  }
}

void PageMainMenu::onInput(InputService::Button btn) {
  auto& s = Settings::get();

  if (btn == InputService::BTN_UP) {
    cursor = (cursor - 1 + ITEM_COUNT) % ITEM_COUNT;
    PageManager::draw();
  }
  else if (btn == InputService::BTN_DOWN) {
    cursor = (cursor + 1) % ITEM_COUNT;
    PageManager::draw();
  }
  else if (btn == InputService::BTN_OK) {
    if (cursor == 0) {
      PageManager::push(&settingsPage);
    }
    else if (cursor == 1) {
      // About 页面（可后续实现）
    }
    else {
      // Exit 页面（可后续实现）
    }
  }
}