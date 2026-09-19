#include "page_main_menu.h"
#include "page_manager.h"
#include "page_settings.h"
#include "page_file_manager.h"
#include "settings.h"
#include "layout.h"

namespace {
  const char* items[PageMainMenu::ITEM_COUNT] = {
    "Settings",
    "Files",
    "About",
    "Exit"
  };
}

void PageMainMenu::onEnter() {
  cursor = 0;
}

void PageMainMenu::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Main Menu");

  for (int i = 0; i < ITEM_COUNT; i++) {
    int col = i % 2;
    int row = i / 2;

    int x0 = Layout::COL_X[col];
    int yc = Layout::ROW_Y[row];

    if (i == cursor) {
      GraphicsAPI::drawRect(x0, yc - Layout::BOX_H / 2,
                            Layout::COL_W, Layout::BOX_H, accent);
    }

    GraphicsAPI::setCursor(x0 + Layout::TEXT_OFF_X,
                           yc + Layout::TEXT_OFF_Y);
    GraphicsAPI::print(items[i]);
  }
}

void PageMainMenu::onInput(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    cursor = (cursor - 1 + ITEM_COUNT) % ITEM_COUNT;
    PageManager::draw();
  }
  else if (btn == InputService::BTN_DOWN) {
    cursor = (cursor + 1) % ITEM_COUNT;
    PageManager::draw();
  }
  else if (btn == InputService::BTN_OK) {
    if (cursor == 0)      PageManager::push(&settingsPage);
    else if (cursor == 1) PageManager::push(&fileManagerPage);
    // cursor == 2 (About)、3 (Exit) 暂未实现
  }
}