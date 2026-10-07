#include "page_manager.h"
#include "display_service.h"
#include "settings.h"
#include <Arduino.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <cstring>

namespace {
  // 正常最深层级：首页(0) → 主菜单(1) → 文件管理(2) → 文本/图片(3) → 删除确认(4)
  // 原来是 4，正好差一层：push(确认框) 被静默丢弃，导致"删除文件"永远弹不出确认框。
  const int MAX_STACK = 8;
  Page* stack[MAX_STACK];
  int   top   = -1;
  bool  dirty = false;
}

namespace PageManager {

  void init() {
    top = -1;
    dirty = false;
  }

  bool push(Page* page) {
    if (page == nullptr) return false;
    if (top + 1 >= MAX_STACK) {
      Serial.println("[page] ERR: stack full, push ignored");
      return false;
    }
    if (top >= 0) stack[top]->onExit();
    stack[++top] = page;
    page->onEnter();
    dirty = true;
    return true;
  }

  void pop() {
    if (top < 0) return;
    stack[top]->onExit();
    top--;
    if (top >= 0) {
      stack[top]->onEnter();
      dirty = true;
    }
  }

  void replace(Page* page) {
    if (page == nullptr) return;
    if (top < 0) { push(page); return; }   // 空栈时退化为 push，避免写 stack[-1]
    stack[top]->onExit();
    stack[top] = page;
    page->onEnter();
    dirty = true;
  }
  
  void resetTo(Page* page) {
    if (page == nullptr) return;
    // 连栈底一起退栈，onExit/onEnter 生命周期才对称
    while (top >= 0) {
      stack[top]->onExit();
      top--;
    }
    top = 0;
    stack[top] = page;
    page->onEnter();
    dirty = true;
  }

  Page* current() {
    return (top >= 0) ? stack[top] : nullptr;
  }

  void markDirty() {
    dirty = true;
  }

  bool isDirty() {
    return dirty;
  }

  void draw() {
    if (top < 0) return;

    auto& s = Settings::get();
    GraphicsAPI::Color accent = GraphicsAPI::RED;

    if (stack[top]->customRender()) {
      stack[top]->onDraw(accent);
      dirty = false;
      return;
    }

    GraphicsAPI::beginFrame(s.rotation);
    GraphicsAPI::clear(GraphicsAPI::WHITE);
    GraphicsAPI::setTextColor(GraphicsAPI::BLACK);
    GraphicsAPI::setFont(NULL);
    stack[top]->onDraw(accent);
    GraphicsAPI::endFrame();
    dirty = false;
  }

  void flushIfDirty() {
    if (dirty) draw();
  }

  void handleInput(InputService::Event evt) {
    if (top < 0) return;
    if (evt.type == InputService::EVT_SHORT) {
      stack[top]->onInput(evt.button);
    } else if (evt.type == InputService::EVT_LONG) {
      stack[top]->onLongPress(evt.button);
    }
  }

}   // ← namespace PageManager 的闭合大括号，务必保留