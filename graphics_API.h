#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>

namespace GraphicsAPI {

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
  void print(const String& str);
  void drawBitmap(int x, int y, const uint8_t* bitmap, int w, int h, Color color);
  void drawRect(int x, int y, int w, int h, Color color);
  void fillRect(int x, int y, int w, int h, Color color);
  void fillCircle(int x, int y, int r, Color color);

  int width();
  int height();

  // ---- 中文接口 ----
  uint16_t utf8_decode(const char* s, int* next);
  int drawChineseChar(int x, int y, uint16_t codepoint, Color color);
  void printUTF8(int x, int y, const char* s, Color color);
}