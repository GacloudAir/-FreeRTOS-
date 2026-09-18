#pragma once
#include "graphics_API.h"
#include "input_service.h"

class Page {
public:
  virtual ~Page() {}

  virtual void onEnter() {}
  virtual void onExit() {}
  virtual void onDraw(GraphicsAPI::Color accent) = 0;
  virtual void onInput(InputService::Button btn) {}       // 短按
  virtual void onLongPress(InputService::Button btn) {}   // 长按
  virtual const char* name() const = 0;
};