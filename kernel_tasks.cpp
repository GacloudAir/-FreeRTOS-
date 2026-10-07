#include "kernel_tasks.h"
#include "input_service.h"
#include "page_manager.h"
#include "settings.h"
#include "storage_service.h"
#include "serial_upload.h"
#include "network_service.h"
#include "time_service.h"
#include "weather_service.h"
#include "display_service.h"
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
  volatile bool     syncRequested = false;   // taskUI 置位，taskBg 消费

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

    bool          firstRun       = true;
    int           lastSyncedHour = -1;
    unsigned long lastAttemptMs  = 0;

    for (;;) {
      bool needSync = false;

      if (firstRun) {
        needSync = true;
        firstRun = false;
      }

      // 手动请求（Clock 页按 UP）优先于任何定时判断
      if (syncRequested) {
        syncRequested = false;
        needSync = true;
      }

      if (!needSync) {
        if (TimeService::isSynced()) {
          // 整点触发
          time_t t = TimeService::now();
          struct tm tmv;
          if (gmtime_r(&t, &tmv) != nullptr && tmv.tm_hour != lastSyncedHour) {
            needSync = true;
          }
        } else if (millis() - lastAttemptMs >= 5UL * 60 * 1000) {
          // 尚未同步成功 → 每 5 分钟退避重试。
          // 原实现只在 isSynced() 为真时才判断整点，一旦同步失败就永远不再自动同步。
          needSync = true;
        }
        // 天气过期触发
        if (!WeatherService::isFresh(55UL * 60 * 1000)) {
          needSync = true;
        }
      }

      if (needSync) {
        lastAttemptMs = millis();
        Serial.println("[bg] sync: connecting...");

        if (NetworkService::connectOnce(20000)) {
          const bool timeOk    = TimeService::syncNow();
          const bool weatherOk = WeatherService::fetch();
          NetworkService::disconnect();

          Serial.print("[bg] time=");
          Serial.print(timeOk ? "ok" : "FAIL");
          Serial.print(" weather=");
          Serial.println(weatherOk ? "ok" : "FAIL");

          if (TimeService::isSynced()) {
            time_t t = TimeService::now();
            struct tm tmv;
            if (gmtime_r(&t, &tmv) != nullptr) lastSyncedHour = tmv.tm_hour;
          }

          // 刷新屏幕
          PageManager::markDirty();
          KernelTasks::requestRefresh();
        } else {
          // 失败：5 分钟后重试
          Serial.println("[bg] sync: wifi connect FAILED, retry in 5 min");
          vTaskDelay(pdMS_TO_TICKS(5UL * 60 * 1000));
          continue;
        }
      }

      // 10 分钟无操作 → 返回 Home
      if (millis() - lastActivityMs > 10UL * 60 * 1000) {
        Page* cur = PageManager::current();
        if (cur && strcmp(cur->name(), "MainPage") != 0) {
          // 只改页面栈并置脏，不在本任务里绘制。
          // resetTo() 已把 dirty 置上，taskUI 会在 500ms 内刷新；
          // 从 taskBg 直接调 draw() 会与 taskUI 并发操作同一块 GFXcanvas。
          PageManager::resetTo(mainPageApp.getPage());
        }
        lastActivityMs = millis();
      }

      // 页面 onTick
      Page* cur = PageManager::current();
      if (cur) cur->onTick();

      // 空闲 60s 后给墨水屏断电（省电）；下次刷新会自动重新上电。
      // 保持上电是为了省掉每帧的"复位 + 重灌 LUT + PowerOff(delay 1.5s)"。
      DisplayService::sleepIfIdle(60UL * 1000);

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

  void requestSync() {
    // 只置位；由 taskBg 在下一轮循环（≤1s）消费，调用者不阻塞。
    syncRequested = true;
  }
}