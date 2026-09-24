#include "app_about.h"
#include "page_manager.h"
#include "layout.h"
#include <Arduino.h>
#include <LittleFS.h>

class PageAbout : public Page {
public:
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override {}
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "About"; }
};

static PageAbout pageInstance;

static Page* getPage() { return &pageInstance; }

const App aboutApp = {
  "about",
  "About",
  getPage
};

void PageAbout::onDraw(GraphicsAPI::Color accent) {
  GraphicsAPI::setCursor(Layout::TITLE_X, Layout::TITLE_Y);
  GraphicsAPI::print("About");

  int x = Layout::COL_X[0] + Layout::TEXT_OFF_X;
  static const int ABOUT_Y[3] = {36, 52, 68};

  // 第一行：版本号
  GraphicsAPI::setCursor(x, ABOUT_Y[0] + Layout::TEXT_OFF_Y);
  GraphicsAPI::print("v0.5.1");

  // 第二行：RAM 空闲
  GraphicsAPI::setCursor(x, ABOUT_Y[1] + Layout::TEXT_OFF_Y);
  GraphicsAPI::print("RAM: ");
  GraphicsAPI::print((int)(rp2040.getFreeHeap() / 1024));
  GraphicsAPI::print("KB");

  // 第三行：Flash 使用情况
  GraphicsAPI::setCursor(x, ABOUT_Y[2] + Layout::TEXT_OFF_Y);
  GraphicsAPI::print("FS: ");

  FSInfo fsinfo;
  if (LittleFS.info(fsinfo)) {
    int pct = (int)(100.0 * fsinfo.usedBytes / fsinfo.totalBytes);
    GraphicsAPI::print(pct);
    GraphicsAPI::print("%");
  } else {
    GraphicsAPI::print("N/A");
  }

  //行外页:长按退出该页面
  GraphicsAPI::setCursor(Layout::TITLE_X, 90);
  GraphicsAPI::print("Hold OK: Back");
}

void PageAbout::onLongPress(InputService::Button btn) {
  if (btn == InputService::BTN_OK) {
    PageManager::pop();
  }
}