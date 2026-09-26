#pragma once
#include "page.h"

class PageMainMenu : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "MainMenu"; }

  // 前 2 项固定（Settings、Files），最后 1 项固定（Exit）
  static const int FIXED_HEAD = 2;
  static const int FIXED_TAIL = 1;

private:
  int cursor    = 0;
  int scrollTop = 0;

  int totalItems() const;
  const char* labelFor(int idx) const;
  void ensureCursorVisible();
};