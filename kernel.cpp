#include "kernel.h"
#include "input_service.h"
#include "display_service.h"
#include "storage_service.h"
#include "settings.h"
#include "page_manager.h"
#include "page_main_menu.h"
#include "serial_upload.h"      // ← 新增
#include <Arduino.h>

// 全局页面实例
static PageMainMenu mainMenu;

void Kernel::init() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("=== System booting ===");

  StorageService::begin();
  Settings::load();

  DisplayService::init();
  InputService::init();
  SerialUpload::init();       // ← 新增

  PageManager::init();
  PageManager::push(&mainMenu);   // push 内部会自动 draw

  Serial.println("=== System ready ===");
}

void Kernel::run() {
  if (SerialUpload::poll()) return;   // ← 新增：正在接收文件时跳过按键处理

  InputService::Event evt = InputService::poll();
  if (evt.type != InputService::EVT_NONE) {
    PageManager::handleInput(evt);
  }
  delay(5);
}