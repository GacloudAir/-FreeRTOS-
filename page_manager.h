#pragma once
#include "page.h"

namespace PageManager {
  void init();
  void push(Page* page);   // 进入新页面
  void pop();              // 返回上一页
  void replace(Page* page);// 替换当前页
  Page* current();
  void draw();             // 绘制当前页
  void handleInput(InputService::Button btn);
}