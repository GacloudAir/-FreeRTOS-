#include "page_manager.h"
#include "display_service.h"
#include "settings.h"
#include <Arduino.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <cstring>

namespace {
  const int MAX_STACK = 4;
  Page* stack[MAX_STACK];
  int   top   = -1;
  bool  dirty = false;
}

namespace PageManager {

  void init() {
    top = -1;
    dirty = false;
  }

  void push(Page* page) {
    if (top + 1 >= MAX_STACK) return;
    if (top >= 0) stack[top]->onExit();
    stack[++top] = page;
    page->onEnter();
    dirty = true;
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
    if (top >= 0) stack[top]->onExit();
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