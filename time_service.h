#pragma once
#include <Arduino.h>
#include <time.h>

namespace TimeService {
  void   start();
  void   poll();
  bool   isSynced();
  time_t now();
  String formatTime();
  String formatDate();
  String formatWeekday();
  void   setTimezoneOffset(int32_t sec);
  int32_t getTimezoneOffset();
}