#pragma once
#include <Arduino.h>

namespace NetworkService {
  void start();                      // 兼容保留，空实现
  void poll();                       // 兼容保留，空实现

  bool connectOnce(unsigned long timeoutMs = 20000);  // 同步连接
  void disconnect();                 // 断开 WiFi

  bool isConnected();
  bool isConfigured();

  String getTimeString();
}