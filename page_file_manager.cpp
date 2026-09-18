#include "page_file_manager.h"
#include "page_manager.h"
#include "settings.h"

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
  if (cursor >= scrollTop + ROWS_PER_PAGE) {
    scrollTop = cursor - ROWS_PER_PAGE + 1;
  }
}

void PageFileManager::onDraw(GraphicsAPI::Color accent) {
  // 标题
  GraphicsAPI::setCursor(20, 20);
  GraphicsAPI::print("Files (");
  GraphicsAPI::print(fileCount);
  GraphicsAPI::print(")");

  if (fileCount == 0) {
    GraphicsAPI::setCursor(40, 70);
    GraphicsAPI::print("(empty)");
    return;
  }

  // 文件列表
  for (int i = 0; i < ROWS_PER_PAGE; i++) {
    int idx = scrollTop + i;
    if (idx >= fileCount) break;

    int y = 50 + i * 22;
    GraphicsAPI::setCursor(30, y);

    // 去掉 LittleFS 的 "/" 前缀并截断
    String name = files[idx].name;
    if (name.startsWith("/")) name = name.substring(1);
    if (name.length() > 22) name = name.substring(0, 19) + "...";
    GraphicsAPI::print(name.c_str());

    if (idx == cursor) {
      GraphicsAPI::drawRect(20, y - 12, 220, 20, accent);
    }
  }

  // 底部页码
  int totalPages = (fileCount + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
  int currentPage = scrollTop / ROWS_PER_PAGE + 1;

  GraphicsAPI::setCursor(20, 175);
  GraphicsAPI::print("[");
  GraphicsAPI::print(currentPage);
  GraphicsAPI::print("/");
  GraphicsAPI::print(totalPages);
  GraphicsAPI::print("]");
}

void PageFileManager::onInput(InputService::Button btn) {
  // 短按：单页内移动光标
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
  else if (btn == InputService::BTN_OK) {
    // TODO: M6 接入文本查看器
  }
}

void PageFileManager::onLongPress(InputService::Button btn) {
  // 长按：整页翻页
  if (btn == InputService::BTN_UP) {
    if (scrollTop >= ROWS_PER_PAGE) {
      scrollTop -= ROWS_PER_PAGE;
      cursor = scrollTop;
      PageManager::draw();
    }
  }
  else if (btn == InputService::BTN_DOWN) {
    if (scrollTop + ROWS_PER_PAGE < fileCount) {
      scrollTop += ROWS_PER_PAGE;
      cursor = scrollTop;
      PageManager::draw();
    }
  }
  else if (btn == InputService::BTN_OK) {
    // 长按 OK：返回上一页
    PageManager::pop();
    PageManager::draw();
  }
}