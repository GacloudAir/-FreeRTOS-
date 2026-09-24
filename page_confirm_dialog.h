#pragma once
#include "page.h"

class PageConfirmDialog : public Page {
public:
  void setup(const char* path);   // 设置要删除的目标

  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Confirm"; }

private:
  char targetPath[80] = {0};
};

extern PageConfirmDialog confirmDialog;