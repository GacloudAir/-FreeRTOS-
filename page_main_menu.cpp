#include "page_main_menu.h"
#include "page_manager.h"
#include "page_settings.h"
#include "page_file_manager.h"
#include "app_registry.h"
#include "layout.h"

void PageMainMenu::onEnter() {
  cursor = 0;
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

void PageMainMenu::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Main Menu");

  int n = totalItems();
  for (int i = 0; i < n; i++) {
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
    GraphicsAPI::print(labelFor(i));
  }
}

void PageMainMenu::onInput(InputService::Button btn) {
  int n = totalItems();

  if (btn == InputService::BTN_UP) {
    cursor = (cursor - 1 + n) % n;
    PageManager::markDirty();
  }
  else if (btn == InputService::BTN_DOWN) {
    cursor = (cursor + 1) % n;
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