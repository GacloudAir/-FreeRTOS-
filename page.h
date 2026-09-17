#pragma once
#include "graphics_API.h"
#include "input_service.h"

class Page {
public:
  virtual ~Page() {}

  // 进入页面时调用一次（可选）
  virtual void onEnter() {}

  // 离开页面时调用一次（可选）
  virtual void onExit() {}

  // 每帧绘制页面内容（由 PageManager 调用 beginFrame/endFrame）
  virtual void onDraw(GraphicsAPI::Color accent) = 0;

  // 处理按键事件
  virtual void onInput(InputService::Button btn) = 0;

  // 页面标题（用于调试或标题栏）
  virtual const char* name() const = 0;
};