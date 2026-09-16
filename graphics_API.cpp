#include "graphics_API.h"
#include "display_service.h"   // 提供 extern display 对象
#include <GxEPD2_3C.h>        // 提供 GxEPD_BLACK / WHITE / RED

namespace {
  uint16_t toGxColor(GraphicsAPI::Color c) {
    switch (c) {
      case GraphicsAPI::BLACK: return GxEPD_BLACK;
      case GraphicsAPI::WHITE: return GxEPD_WHITE;
      case GraphicsAPI::RED:   return GxEPD_RED;
      default:                 return GxEPD_BLACK;
    }
  }
}

namespace GraphicsAPI {

  void beginFrame(int rotation) {
    DisplayService::beginFrame(rotation);
  }

  void endFrame() {
    DisplayService::endFrame();
  }

  void clear(Color color) {
    display.fillScreen(toGxColor(color));
  }

  void setTextColor(Color color) {
    display.setTextColor(toGxColor(color));
  }

  void setFont(const GFXfont* font) {
    display.setFont(font);
  }

  void setCursor(int x, int y) {
    display.setCursor(x, y);
  }

  void print(const char* str) {
    display.print(str);
  }

  void print(int num) {
    display.print(num);
  }

  void drawRect(int x, int y, int w, int h, Color color) {
    display.drawRect(x, y, w, h, toGxColor(color));
  }

  void fillCircle(int x, int y, int r, Color color) {
    display.fillCircle(x, y, r, toGxColor(color));
  }
}