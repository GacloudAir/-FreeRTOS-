#pragma once
#include <Arduino.h>

namespace InputService {
  enum Button {
    BTN_NONE,
    BTN_UP,
    BTN_DOWN,
    BTN_OK
  };

  void init();
  Button poll();   // 每次调用返回一个按键事件，无按键返回 BTN_NONE
}