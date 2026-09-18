#pragma once
#include "page.h"

namespace PageManager {
  void init();
  void push(Page* page);
  void pop();
  void replace(Page* page);
  Page* current();
  void draw();

  // 注意：这里改成 Event
  void handleInput(InputService::Event evt);
}