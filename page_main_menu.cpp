#include "page_main_menu.h"
#include "page_manager.h"
#include "page_settings.h"
#include "page_file_manager.h"
#include "app_registry.h"
#include "layout.h"
#include <string.h>

void PageMainMenu::onEnter() {
  cursor = 0;
  scrollTop = 0;
}

int PageMainMenu::totalItems() const {
  return FIXED_HEAD + AppRegistry::count() + FIXED_TAIL;
}

const char* PageMainMenu::labelFor(int idx) const {
  if (idx == 0) return "Settings";
  if (idx == 1) return "Files";
  if (idx == totalItems() - 1) return "Exit";
  return AppRegistry::get(idx - FIXED_HEAD).name;
}

void PageMainMenu::ensureCursorVisible() {
  if (cursor < scrollTop) scrollTop = cursor;
  if (cursor >= scrollTop + Layout::ITEMS_PER_PAGE) {
    scrollTop = cursor - Layout::ITEMS_PER_PAGE + 1;
  }
}

void PageMainMenu::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Main Menu");

  int n = totalItems();

  // 右上角页码（仅多页时显示）
  int totalPages = (n + Layout::ITEMS_PER_PAGE - 1) / Layout::ITEMS_PER_PAGE;
  if (totalPages > 1) {
    int currentPage = scrollTop / Layout::ITEMS_PER_PAGE + 1;
    GraphicsAPI::setCursor(175, Layout::TITLE_Y);
    GraphicsAPI::print("[");
    GraphicsAPI::print(currentPage);
    GraphicsAPI::print("/");
    GraphicsAPI::print(totalPages);
    GraphicsAPI::print("]");
  }

  // 渲染当前页最多 6 项
  for (int slot = 0; slot < Layout::ITEMS_PER_PAGE; slot++) {
    int idx = scrollTop + slot;
    if (idx >= n) break;

    int col = slot % 2;
    int row = slot / 2;
    int x0 = Layout::COL_X[col];
    int yc = Layout::ROW_Y[row];

    if (idx == cursor) {
      GraphicsAPI::drawRect(x0, yc - Layout::BOX_H / 2,
                            Layout::COL_W, Layout::BOX_H, accent);
    }

    GraphicsAPI::setCursor(x0 + Layout::TEXT_OFF_X,
                           yc + Layout::TEXT_OFF_Y);
    GraphicsAPI::print(labelFor(idx));
  }
}

void PageMainMenu::onInput(InputService::Button btn) {
  int n = totalItems();

  if (btn == InputService::BTN_UP) {
    cursor = (cursor - 1 + n) % n;
    ensureCursorVisible();
    PageManager::markDirty();
  }
  else if (btn == InputService::BTN_DOWN) {
    cursor = (cursor + 1) % n;
    ensureCursorVisible();
    PageManager::markDirty();
  }
  else if (btn == InputService::BTN_OK) {
    if (cursor == 0) {
      PageManager::push(&settingsPage);
    }
    else if (cursor == 1) {
      PageManager::push(&fileManagerPage);
    }
    else if (cursor == n - 1) {
      // Exit：暂未实现
    }
    else {
      const App& app = AppRegistry::get(cursor - FIXED_HEAD);
      Page* p = app.getPage();
      if (p) PageManager::push(p);
    }
  }
}

void PageMainMenu::onLongPress(InputService::Button btn) {
  int n = totalItems();
  int maxScroll = (n > Layout::ITEMS_PER_PAGE) ? n - Layout::ITEMS_PER_PAGE : 0;

  if (btn == InputService::BTN_UP) {
    if (scrollTop >= Layout::ITEMS_PER_PAGE) {
      scrollTop -= Layout::ITEMS_PER_PAGE;
    } else {
      scrollTop = 0;
    }
    cursor = scrollTop;
    PageManager::markDirty();
  }
  else if (btn == InputService::BTN_DOWN) {
    if (scrollTop + Layout::ITEMS_PER_PAGE < n) {
      scrollTop += Layout::ITEMS_PER_PAGE;
      if (scrollTop > maxScroll) scrollTop = maxScroll;
    } else {
      scrollTop = maxScroll;
    }
    cursor = scrollTop;
    PageManager::markDirty();
  }
}