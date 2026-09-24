#pragma once
#include <Arduino.h>

namespace Settings {

  constexpr int REFRESH_BLOCKING = 0;
  constexpr int REFRESH_SMART    = 1;

  struct Data {
    int version;
    int page;
    int rotation;
    int refreshMode;   // 0=阻塞, 1=智能
  };

  Data& get();
  void  load();
  bool  save();
  void  resetToDefaults();
}