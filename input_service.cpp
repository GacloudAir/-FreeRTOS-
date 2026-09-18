#include "input_service.h"

namespace {
  const int PIN_UP   = 11;
  const int PIN_DOWN = 12;
  const int PIN_OK   = 13;
  const unsigned long LONG_PRESS_MS = 800;

  int lastButton = InputService::BTN_NONE;
  unsigned long pressStartTime = 0;
  bool longPressFired = false;
  bool wasPressed = false;

  int readCurrentPressed() {
    if (digitalRead(PIN_UP)   == LOW) return InputService::BTN_UP;
    if (digitalRead(PIN_DOWN) == LOW) return InputService::BTN_DOWN;
    if (digitalRead(PIN_OK)   == LOW) return InputService::BTN_OK;
    return InputService::BTN_NONE;
  }
}

namespace InputService {

  void init() {
    pinMode(PIN_UP,   INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_OK,   INPUT_PULLUP);
  }

  Event poll() {
    Event evt = { BTN_NONE, EVT_NONE };
    int current = readCurrentPressed();
    unsigned long now = millis();

    if (current != BTN_NONE && !wasPressed) {
      // 按下瞬间
      wasPressed = true;
      lastButton = current;
      pressStartTime = now;
      longPressFired = false;
    }
    else if (current != BTN_NONE && wasPressed && !longPressFired) {
      // 持续按住
      if (now - pressStartTime >= LONG_PRESS_MS) {
        longPressFired = true;
        evt.button = (Button)current;
        evt.type = EVT_LONG;
      }
    }
    else if (current == BTN_NONE && wasPressed) {
      // 松开
      wasPressed = false;
      if (!longPressFired && lastButton != BTN_NONE) {
        evt.button = (Button)lastButton;
        evt.type = EVT_SHORT;
      }
      lastButton = BTN_NONE;
    }

    return evt;
  }
}