#include "kernel_tasks.h"
#include "input_service.h"
#include "page_manager.h"
#include "settings.h"
#include "storage_service.h"
#include "serial_upload.h"
#include "network_service.h"

#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

namespace {
  QueueHandle_t     eventQueue = nullptr;
  SemaphoreHandle_t refreshSem = nullptr;

  void taskInput(void* pv) {
    InputService::init();
    for (;;) {
      InputService::Event evt = InputService::poll();
      if (evt.type != InputService::EVT_NONE) {
        xQueueSend(eventQueue, &evt, 0);
      }
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }

  void taskUI(void* pv) {
    unsigned long lastEventTime = 0;
    bool hasPending = false;
    bool lastWasLong = false;

    for (;;) {
      InputService::Event evt;
      bool gotEvent = false;

      if (xQueueReceive(eventQueue, &evt, pdMS_TO_TICKS(20)) == pdTRUE) {
        PageManager::handleInput(evt);
        lastEventTime = millis();
        gotEvent = true;
        hasPending = true;
        lastWasLong = (evt.type == InputService::EVT_LONG);
      }

      auto& s = Settings::get();

      if (s.refreshMode == Settings::REFRESH_BLOCKING) {
        if (gotEvent) {
          PageManager::flushIfDirty();
          hasPending = false;
        }
      } else {
        if (hasPending) {
          if (lastWasLong || (millis() - lastEventTime > 400)) {
            PageManager::flushIfDirty();
            hasPending = false;
            lastWasLong = false;
          }
        }
      }
    }
  }

  void taskBg(void* pv) {
    vTaskDelay(pdMS_TO_TICKS(5000));   // 等 ESP8285 启动
    for (;;) {
      NetworkService::poll();
      vTaskDelay(pdMS_TO_TICKS(300));
    }
  }

  // 串口配置任务
  void taskSerial(void* pv) {
    for (;;) {
      SerialUpload::poll();
      vTaskDelay(pdMS_TO_TICKS(20));
    }
  }
}

namespace KernelTasks {

  void begin() {
    eventQueue = xQueueCreate(16, sizeof(InputService::Event));
    refreshSem = xSemaphoreCreateBinary();

    xTaskCreate(taskInput,  "Input",  512,  nullptr, 3, nullptr);
    xTaskCreate(taskUI,     "UI",     2048, nullptr, 2, nullptr);
    xTaskCreate(taskBg,     "Bg",     4096, nullptr, 1, nullptr);
    xTaskCreate(taskSerial, "Serial", 2048, nullptr, 1, nullptr);
  }

  void requestRefresh() {
    if (refreshSem) xSemaphoreGive(refreshSem);
  }
}