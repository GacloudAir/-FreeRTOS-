#pragma once
#include "page.h"

class PageTextViewer : public Page {
public:
  // 打开文件，成功返回 true
  bool open(const char* path);

  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "TextViewer"; }

  static const int MAX_LINES     = 200;
  static const int LINES_VISIBLE = 7;
  static const int LINE_LEN      = 17;

private:
  char  buffer[8192];
  int   lineOffset[MAX_LINES];
  int   lineLength[MAX_LINES];
  int   lineCount = 0;
  int   topLine   = 0;
  char  title[20] = {0};
  char  currentPath[80] = {0};

  void parseLines();
};

extern PageTextViewer textViewerPage;