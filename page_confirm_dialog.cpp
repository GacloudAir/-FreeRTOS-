#include "page_confirm_dialog.h"
#include "page_manager.h"
#include "storage_service.h"
#include "layout.h"
#include <string.h>

PageConfirmDialog confirmDialog;

void PageConfirmDialog::setup(const char* path) {
  strncpy(targetPath, path, sizeof(targetPath) - 1);
  targetPath[sizeof(targetPath) - 1] = '\0';
}

void PageConfirmDialog::onDraw(GraphicsAPI::Color accent) {
  // 居中弹出框：宽 160，高 50
  int boxW = 160, boxH = 50;
  int boxX = (212 - boxW) / 2;
  int boxY = (104 - boxH) / 2;

  // 外框
  GraphicsAPI::drawRect(boxX, boxY, boxW, boxH, accent);
  GraphicsAPI::drawRect(boxX + 1, boxY + 1, boxW - 2, boxH - 2, accent);

  // 提示文字
  GraphicsAPI::setCursor(boxX + 20, boxY + 18);
  GraphicsAPI::print("Confirm delete?");

  GraphicsAPI::setCursor(boxX + 20, boxY + 34);
  GraphicsAPI::print("Hold OK: Yes");
}

void PageConfirmDialog::onInput(InputService::Button btn) {
  // 短按任意键 → 取消，返回上一页
  (void)btn;
  PageManager::pop();
}

void PageConfirmDialog::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    StorageService::remove(targetPath);
    PageManager::pop();   // 弹出对话框
    PageManager::pop();   // 弹出查看器，返回 Files 页
  }
}