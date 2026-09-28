#include "kernel.h"
#include "kernel_tasks.h"
#include "input_service.h"
#include "display_service.h"
#include "storage_service.h"
#include "settings.h"
#include "page_manager.h"
#include "page_main_menu.h"
#include "app_main_page.h"
#include "serial_upload.h"
#include "network_service.h"
#include "time_service.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

void Kernel::init() {
  Serial.begin(115200);
  delay(2000);

  StorageService::begin();
  Settings::load();
  DisplayService::init();

  PageManager::init();
  PageManager::resetTo(mainPageApp.getPage());

  SerialUpload::init();
  KernelTasks::begin();
}

void Kernel::run() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}