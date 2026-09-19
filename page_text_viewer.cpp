#include "page_text_viewer.h"
#include "page_manager.h"
#include "storage_service.h"
#include <string.h>

PageTextViewer textViewerPage;

bool PageTextViewer::open(const char* path) {
  String content = StorageService::read(path);
  if (content.length() == 0) return false;

  int totalBytes = content.length();
  if (totalBytes > (int)sizeof(buffer) - 1) {
    totalBytes = sizeof(buffer) - 1;
  }
  memcpy(buffer, content.c_str(), totalBytes);
  buffer[totalBytes] = '\0';

  // 标题：取路径最后一段
  const char* slash = strrchr(path, '/');
  const char* fname = slash ? slash + 1 : path;
  strncpy(title, fname, sizeof(title) - 1);
  title[sizeof(title) - 1] = '\0';

  parseLines();
  topLine = 0;
  return lineCount > 0;
}

void PageTextViewer::parseLines() {
  lineCount = 0;
  int i = 0;
  int total = strlen(buffer);

  while (i < total && lineCount < MAX_LINES) {
    // 跳过行首空白与换行
    while (i < total && (buffer[i] == '\r' || buffer[i] == '\n')) i++;
    if (i >= total) break;

    lineOffset[lineCount] = i;

    // 按像素宽度累计，204px 为可视宽度上限
    int bytes = 0;
    int pixelW = 0;
    while (i + bytes < total &&
           buffer[i + bytes] != '\r' &&
           buffer[i + bytes] != '\n') {
      uint8_t c = (uint8_t)buffer[i + bytes];
      int charBytes, charW;
      if (c < 0x80) {
        charBytes = 1; charW = 6;
      } else if ((c & 0xE0) == 0xC0) {
        charBytes = 2; charW = 12;
      } else if ((c & 0xF0) == 0xE0) {
        charBytes = 3; charW = 12;
      } else {
        charBytes = 1; charW = 6;
      }
      if (pixelW + charW > 204) break;
      pixelW += charW;
      bytes += charBytes;
    }
    lineLength[lineCount] = bytes;

    // 跳过本行剩余部分（超长截断）
    i += bytes;
    while (i < total && buffer[i] != '\r' && buffer[i] != '\n') i++;

    lineCount++;
  }
}

void PageTextViewer::onEnter() {
  // open() 已经重置 topLine
}

void PageTextViewer::onDraw(GraphicsAPI::Color accent) 
{
  // 标题栏
  GraphicsAPI::setCursor(4, 8);
  GraphicsAPI::print(title);

  int totalPages = (lineCount + LINES_VISIBLE - 1) / LINES_VISIBLE;
  if (totalPages < 1) totalPages = 1;

  int currentPage;
  if (lineCount <= LINES_VISIBLE) {
    currentPage = 1;
  } else if (topLine + LINES_VISIBLE >= lineCount) {
    currentPage = totalPages;      // 最后一页直接取 totalPages
  } else {
    currentPage = topLine / LINES_VISIBLE + 1;
  }

  GraphicsAPI::setCursor(175, 8);
  GraphicsAPI::print("[");
  GraphicsAPI::print(currentPage);
  GraphicsAPI::print("/");
  GraphicsAPI::print(totalPages);
  GraphicsAPI::print("]");

  // 分隔线
  GraphicsAPI::drawRect(0, 17, 212, 1, accent);

  if (lineCount == 0) {
    GraphicsAPI::setCursor(10, 50);
    GraphicsAPI::print("(empty)");
    return;
  }

  // 正文：行高 12px，从 y=20 开始
  for (int i = 0; i < LINES_VISIBLE; i++) {
    int idx = topLine + i;
    if (idx >= lineCount) break;

    char tmp[LINE_LEN * 3 + 1];
    int len = lineLength[idx];
    memcpy(tmp, buffer + lineOffset[idx], len);
    tmp[len] = '\0';

    GraphicsAPI::printUTF8(4, 20 + i * 12, tmp, GraphicsAPI::BLACK);
  }
}

void PageTextViewer::onInput(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    if (topLine > 0) {
      topLine--;
      PageManager::draw();
    }
  }
  else if (btn == InputService::BTN_DOWN) {
    if (topLine + LINES_VISIBLE < lineCount) {
      topLine++;
      PageManager::draw();
    }
  }
  // 短按 OK 无操作
}

void PageTextViewer::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_UP) {
    if (topLine >= LINES_VISIBLE) topLine -= LINES_VISIBLE;
    else                          topLine = 0;
    PageManager::draw();
  }
  else if (btn == InputService::BTN_DOWN) {
    topLine += LINES_VISIBLE;
    if (topLine + LINES_VISIBLE > lineCount) {
      topLine = lineCount - LINES_VISIBLE;
      if (topLine < 0) topLine = 0;
    }
    PageManager::draw();
  }
  else if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}