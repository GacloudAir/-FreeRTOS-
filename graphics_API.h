#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>   // 提供 GFXfont 类型

namespace GraphicsAPI {

  // 颜色枚举，避免 kernel 直接依赖 GxEPD2
  enum Color : uint16_t {
    BLACK,
    WHITE,
    RED
  };

  void beginFrame(int rotation);
  void endFrame();

  void clear(Color color);
  void setTextColor(Color color);
  void setFont(const GFXfont* font);
  void setCursor(int x, int y);
  void print(const char* str);
  void print(int num);

  void drawRect(int x, int y, int w, int h, Color color);
  void fillCircle(int x, int y, int r, Color color);
  // 后续可继续添加 drawLine、drawPixel、drawBitmap 等
}