#include "kernel.h"
#include "input_service.h"
#include "display_service.h"
#include "storage_service.h"
#include "settings.h"
#include "page_manager.h"      // ← 原来可能是 "ui/page_manager.h"
#include "page_main_menu.h"    // ← 原来可能是 "ui/page_main_menu.h"
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

  PageManager::init();
  PageManager::push(&mainMenu);
  PageManager::draw();

  Serial.println("=== System ready ===");
}

void Kernel::run() {
  InputService::Button btn = InputService::poll();
  if (btn != InputService::BTN_NONE) {
    PageManager::handleInput(btn);
  }
  delay(20);
}