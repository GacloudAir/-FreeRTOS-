#pragma once
#include "page.h"

class PageSettings : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Settings"; }

  static const int ITEM_COUNT = 2;   // Rotation, Refresh

private:
  int cursor = 0;
};

extern PageSettings settingsPage;