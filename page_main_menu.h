#pragma once
#include "page.h"

class PageMainMenu : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  const char* name() const override { return "MainMenu"; }

  static const int ITEM_COUNT = 4;   // ← 移到 public

private:
  int cursor = 0;
};