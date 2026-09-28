#pragma once
#include <Arduino.h>
#include <time.h>

namespace TimeService {
  void   start();
  void   poll();               // 兼容保留，空实现
  bool   syncNow();            // 同步阻塞，需 WiFi 已连接
  bool   isSynced();
  time_t now();
  String formatTime();
  String formatDate();
  String formatWeekday();
  void   setTimezoneOffset(int32_t sec);
  int32_t getTimezoneOffset();
}