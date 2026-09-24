#include "page_image_viewer.h"
#include "page_manager.h"
#include "bmp_reader.h"
#include <string.h>
#include "display_service.h"
#include "page_confirm_dialog.h"

PageImageViewer imageViewerPage;

bool PageImageViewer::open(const char* path) {
  loaded = bmp_load_1bit(path, bitmap, IMG_W, IMG_H);
  grayMode = false;

  strncpy(currentPath, path, sizeof(currentPath) - 1);
  currentPath[sizeof(currentPath) - 1] = '\0';// 保存路径
  
  if (!loaded) {
    // 1-bit 失败，尝试 8-bit 灰度
    loaded = bmp_load_8bit_gray(path, grayBitmap, IMG_W, IMG_H);
    grayMode = true;
  }
  
  if (!loaded) return false;

  const char* slash = strrchr(path, '/');
  const char* fname = slash ? slash + 1 : path;
  strncpy(title, fname, sizeof(title) - 1);
  title[sizeof(title) - 1] = '\0';

  return true;
}

void PageImageViewer::onDraw(GraphicsAPI::Color accent) {
  if (!loaded) return;
  
  if (grayMode) {
    DisplayService::flushGray(grayBitmap);
  } else {
    GraphicsAPI::drawBitmap(0, 0, bitmap, IMG_W, IMG_H, GraphicsAPI::BLACK);
  }
}

void PageImageViewer::onInput(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}

void PageImageViewer::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    confirmDialog.setup(currentPath);
    PageManager::push(&confirmDialog);
  }
}