#include "page_file_manager.h"
#include "page_manager.h"
#include "settings.h"
#include "layout.h"
#include "page_text_viewer.h"
#include "page_text_viewer.h"
#include "page_image_viewer.h"

PageFileManager fileManagerPage;

void PageFileManager::onEnter() {
  loadFiles();
  cursor = 0;
  scrollTop = 0;
}

void PageFileManager::loadFiles() {
  fileCount = StorageService::listDir("/", files, MAX_FILES);
}

void PageFileManager::ensureCursorVisible() {
  if (cursor < scrollTop) scrollTop = cursor;
  if (cursor >= scrollTop + Layout::ITEMS_PER_PAGE) {
    scrollTop = cursor - Layout::ITEMS_PER_PAGE + 1;
  }
}

void PageFileManager::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("Files (");
  GraphicsAPI::print(fileCount);
  GraphicsAPI::print(")");

  // 右上角显示页码
  int totalPages = (fileCount + Layout::ITEMS_PER_PAGE - 1) / Layout::ITEMS_PER_PAGE;
  int currentPage = scrollTop / Layout::ITEMS_PER_PAGE + 1;

  GraphicsAPI::setCursor(170, Layout::TITLE_Y);
  GraphicsAPI::print("[");
  GraphicsAPI::print(currentPage);
  GraphicsAPI::print("/");
  GraphicsAPI::print(totalPages);
  GraphicsAPI::print("]");

  if (fileCount == 0) {
    GraphicsAPI::setCursor(Layout::COL_X[0] + Layout::TEXT_OFF_X,
                           Layout::ROW_Y[1] + Layout::TEXT_OFF_Y);
    GraphicsAPI::print("(empty)");
    return;
  }

  for (int i = 0; i < Layout::ITEMS_PER_PAGE; i++) {
    int idx = scrollTop + i;
    if (idx >= fileCount) break;

    int col = i % 2;
    int row = i / 2;
    int x0 = Layout::COL_X[col];
    int yc = Layout::ROW_Y[row];

    if (idx == cursor) {
      GraphicsAPI::drawRect(x0, yc - Layout::BOX_H / 2,
                            Layout::COL_W, Layout::BOX_H, accent);
    }

    GraphicsAPI::setCursor(x0 + Layout::TEXT_OFF_X,
                           yc + Layout::TEXT_OFF_Y);

    String name = files[idx].name;
    if (name.startsWith("/")) name = name.substring(1);
    if (name.length() > 11) name = name.substring(0, 10) + ".";
    GraphicsAPI::print(name.c_str());
  }
}

void PageFileManager::onInput(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    if (cursor > 0) cursor--;
    ensureCursorVisible();
    PageManager::draw();
  }
  else if (btn == InputService::BTN_DOWN) {
    if (cursor + 1 < fileCount) cursor++;
    ensureCursorVisible();
    PageManager::draw();
  }
  else if (btn == InputService::BTN_OK) 
  {
    if (cursor >= fileCount) return;
    String name = files[cursor].name;
    String lower = name;
    lower.toLowerCase();

    if (lower.endsWith(".bmp")) 
    {
      if (imageViewerPage.open(name.c_str())) {
        PageManager::push(&imageViewerPage);
      }
    } else 
    {
      if (textViewerPage.open(name.c_str())) {
        PageManager::push(&textViewerPage);
      }
    }
  }
}

void PageFileManager::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    if (scrollTop >= Layout::ITEMS_PER_PAGE) {
      scrollTop -= Layout::ITEMS_PER_PAGE;
      cursor = scrollTop;
      PageManager::draw();
    }
  }
  else if (btn == InputService::BTN_DOWN) {
    if (scrollTop + Layout::ITEMS_PER_PAGE < fileCount) {
      scrollTop += Layout::ITEMS_PER_PAGE;
      cursor = scrollTop;
      PageManager::draw();
    }
  }
  else if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}