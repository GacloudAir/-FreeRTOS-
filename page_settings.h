#pragma once
#include "page.h"

class PageSettings : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  const char* name() const override { return "Settings"; }

  static const int ITEM_COUNT = 3;   // ← 移到 public

private:
  int cursor = 0;
};

extern PageSettings settingsPage;