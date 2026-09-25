#pragma once
#include "page.h"
#include "input_service.h"

namespace PageManager {
  void init();
  void push(Page* page);
  void pop();
  void replace(Page* page);
  Page* current();

  void draw();            // 立即绘制（不检查 dirty）
  void markDirty();       // 标记需要重绘
  bool isDirty();
  void flushIfDirty();    // 若 dirty 则 draw，然后清除标志

  void handleInput(InputService::Event evt);
}