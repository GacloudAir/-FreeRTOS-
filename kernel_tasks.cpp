#include "kernel_tasks.h"
#include "input_service.h"
#include "page_manager.h"
#include "settings.h"
#include "storage_service.h"
#include "serial_upload.h"
#include "network_service.h"
#include "time_service.h"
#include "weather_service.h"
#include "app_main_page.h"
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <string.h>

namespace {
  QueueHandle_t     eventQueue = nullptr;
  SemaphoreHandle_t refreshSem = nullptr;
  unsigned long     lastActivityMs = 0;

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
  lastActivityMs = millis();
  unsigned long lastEventTime = 0;
  bool hasPending = false;
  bool lastWasLong = false;

  // 开机后主动绘制一帧
  vTaskDelay(pdMS_TO_TICKS(200));
  PageManager::flushIfDirty();

  unsigned long lastDirtyCheck = millis();

  for (;;) {
    InputService::Event evt;
    bool gotEvent = false;

    if (xQueueReceive(eventQueue, &evt, pdMS_TO_TICKS(20)) == pdTRUE) {
      lastActivityMs = millis();
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

    // 每 500ms 检查一次 dirty，不管信号量是否到达
    if (millis() - lastDirtyCheck > 500) {
      lastDirtyCheck = millis();
      PageManager::flushIfDirty();
    }

    // 信号量作为快速通道（可选保留）
    if (xSemaphoreTake(refreshSem, 0) == pdTRUE) {
      PageManager::flushIfDirty();
      hasPending = false;
      lastWasLong = false;
      lastDirtyCheck = millis();
    }
  }
}

  void taskBg(void* pv) {
    vTaskDelay(pdMS_TO_TICKS(5000));   // 等 ESP8285 就绪
    lastActivityMs = millis();

    bool firstRun = true;
    int  lastSyncedHour = -1;

    for (;;) {
      bool needSync = false;

      if (firstRun) {
        needSync = true;
        firstRun = false;
      } else {
        // 整点触发
        if (TimeService::isSynced()) {
          time_t t = TimeService::now();
          int hour = gmtime(&t)->tm_hour;
          if (hour != lastSyncedHour) needSync = true;
        }
        // 天气过期触发
        if (!WeatherService::isFresh(55UL * 60 * 1000)) {
          needSync = true;
        }
      }

      if (needSync) {
        if (NetworkService::connectOnce(20000)) {
          TimeService::syncNow();
          WeatherService::fetch();
          NetworkService::disconnect();

          if (TimeService::isSynced()) {
            time_t t = TimeService::now();
            lastSyncedHour = gmtime(&t)->tm_hour;
          }

          // 刷新屏幕
          PageManager::markDirty();
          KernelTasks::requestRefresh();
        } else {
          // 失败：5 分钟后重试
          vTaskDelay(pdMS_TO_TICKS(5UL * 60 * 1000));
          continue;
        }
      }

      // 10 分钟无操作 → 返回 Home
      if (millis() - lastActivityMs > 10UL * 60 * 1000) {
        Page* cur = PageManager::current();
        if (cur && strcmp(cur->name(), "MainPage") != 0) {
          PageManager::resetTo(mainPageApp.getPage());
          PageManager::draw();
        }
        lastActivityMs = millis();
      }

      // 页面 onTick
      Page* cur = PageManager::current();
      if (cur) cur->onTick();

      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }

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