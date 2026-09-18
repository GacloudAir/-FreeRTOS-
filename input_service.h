#pragma once
#include <Arduino.h>

namespace InputService {
  enum Button {
    BTN_NONE = 0,
    BTN_UP,
    BTN_DOWN,
    BTN_OK
  };

  enum EventType {
    EVT_NONE = 0,
    EVT_SHORT,
    EVT_LONG
  };

  struct Event {
    Button button;
    EventType type;
  };

  void init();
  Event poll();
}