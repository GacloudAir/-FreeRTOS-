#pragma once
#include "page.h"
#include "storage_service.h"
#include "layout.h"  


class PageFileManager : public Page {
public:
  void onEnter() override;
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "Files"; }

  static const int MAX_FILES     = 50;
  static const int ROWS_PER_PAGE = Layout::ITEMS_PER_PAGE;

private:
  StorageService::FileInfo files[MAX_FILES];
  int fileCount = 0;
  int cursor    = 0;
  int scrollTop = 0;

  void loadFiles();
  void ensureCursorVisible();
};

extern PageFileManager fileManagerPage;