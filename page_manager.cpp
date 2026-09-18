#include "page_manager.h"
#include "display_service.h"
#include "settings.h"
#include <Fonts/FreeMonoBold9pt7b.h>

namespace {
  const int MAX_STACK = 4;
  Page* stack[MAX_STACK];
  int   top = -1;
}

namespace PageManager {

  void init() { top = -1; }

  void push(Page* page) {
    if (top + 1 >= MAX_STACK) return;
    if (top >= 0) stack[top]->onExit();
    stack[++top] = page;
    page->onEnter();
  }

  void pop() {
    if (top < 0) return;
    stack[top]->onExit();
    top--;
    if (top >= 0) stack[top]->onEnter();
  }

  void replace(Page* page) {
    if (top >= 0) stack[top]->onExit();
    stack[top] = page;
    page->onEnter();
  }

  Page* current() {
    return (top >= 0) ? stack[top] : nullptr;
  }

  void draw() {
    if (top < 0) return;
    auto& s = Settings::get();

    DisplayService::beginFrame(s.rotation);
    GraphicsAPI::clear(GraphicsAPI::WHITE);
    GraphicsAPI::setTextColor(GraphicsAPI::BLACK);
    GraphicsAPI::setFont(&FreeMonoBold9pt7b);

    GraphicsAPI::Color accent =
      s.colorMode ? GraphicsAPI::RED : GraphicsAPI::BLACK;

    stack[top]->onDraw(accent);

    DisplayService::endFrame();
  }

    void handleInput(InputService::Event evt)
  {
    if (top < 0) return;
    if (evt.type == InputService::EVT_SHORT) {
      stack[top]->onInput(evt.button);
    } else if (evt.type == InputService::EVT_LONG) {
      stack[top]->onLongPress(evt.button);
    }
  }
}