#include "input_service.h"

namespace {
  const int PIN_UP   = 11;
  const int PIN_DOWN = 12;
  const int PIN_OK   = 13;
  unsigned long lastPress = 0;
}

namespace InputService {

  void init() {
    pinMode(PIN_UP,   INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_OK,   INPUT_PULLUP);
  }

  Button poll() {
    if (millis() - lastPress < 300) return BTN_NONE;  // 消抖

    if (digitalRead(PIN_UP) == LOW)   { lastPress = millis(); return BTN_UP; }
    if (digitalRead(PIN_DOWN) == LOW) { lastPress = millis(); return BTN_DOWN; }
    if (digitalRead(PIN_OK) == LOW)   { lastPress = millis(); return BTN_OK; }
    return BTN_NONE;
  }
}