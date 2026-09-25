#pragma once
#include <Arduino.h>

namespace NetworkService {
  void   start();
  void   poll();
  bool   isConnected();
  bool   isConfigured();     // ← 新增：是否已配置 WiFi
  String getTimeString();

  bool   lockUart(unsigned long timeoutMs = 0);
  void   unlockUart();
}