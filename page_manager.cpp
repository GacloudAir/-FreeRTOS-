#include "page_manager.h"
#include "display_service.h"
#include "settings.h"
#include <Fonts/FreeMonoBold9pt7b.h>

namespace {
  const int MAX_STACK = 4;
  Page* stack[MAX_STACK];
  int   top = -1;
}

namespace PageManager 
{

  void init() { top = -1; }

  void push(Page* page) 
  {
    if (top + 1 >= MAX_STACK) return;
    if (top >= 0) stack[top]->onExit();
    stack[++top] = page;
    page->onEnter();
    draw();   // ← 新增：立即刷新新页面
  }

  void pop() 
  {
    if (top < 0) return;
    stack[top]->onExit();
    top--;
    if (top >= 0) {
      stack[top]->onEnter();
      draw();   // ← 新增：立即刷新返回后的页面
    }
  }

  void replace(Page* page) 
  {
    if (top >= 0) stack[top]->onExit();
    stack[top] = page;
    page->onEnter();
    draw();   // ← 新增
  }

  Page* current() {
    return (top >= 0) ? stack[top] : nullptr;
  }

  void draw() 
  {
    if (top < 0) return;
    auto& s = Settings::get();
    GraphicsAPI::Color accent = GraphicsAPI::RED;

    // 页面自定义渲染：直接调用 onDraw，不走 canvas
    if (stack[top]->customRender()) {
      stack[top]->onDraw(accent);
      return;
    }

    // 常规页面：走 canvas 抽象层
    GraphicsAPI::beginFrame(s.rotation);
    GraphicsAPI::clear(GraphicsAPI::WHITE);
    GraphicsAPI::setTextColor(GraphicsAPI::BLACK);
    GraphicsAPI::setFont(NULL);
    stack[top]->onDraw(accent);
    GraphicsAPI::endFrame();
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