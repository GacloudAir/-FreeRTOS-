#include "graphics_API.h"
#include "display_service.h"
#include <Adafruit_GFX.h>
#include <string.h>
#include "font_cn_12x12.h"

namespace 
{
  GFXcanvas1 canvas(EPD_WIDTH, EPD_HEIGHT);

  uint16_t toGfxColor(GraphicsAPI::Color c) 
  {
    switch (c) {
      case GraphicsAPI::BLACK: return 0;
      case GraphicsAPI::WHITE: return 1;
      case GraphicsAPI::RED:   return 0;
    }
    return 0;
  }
  const CnGlyph* findGlyph(uint16_t cp) 
  {
    int lo = 0, hi = font_cn_count - 1;
    while (lo <= hi) 
    {
      int mid = (lo + hi) / 2;
      if (font_cn_table[mid].codepoint == cp) return &font_cn_table[mid];
      if (font_cn_table[mid].codepoint < cp) lo = mid + 1;
      else hi = mid - 1;
    }
    return nullptr;
  }
}

namespace GraphicsAPI 
{
  void drawBitmap(int x, int y, const uint8_t* bitmap,int w, int h, Color color) 
  {
  uint16_t c = toGfxColor(color);
  // GFXcanvas1::drawBitmap 对 color=0 会用 textcolor 兜底，需提前设置
  if (c == 0) canvas.setTextColor(0);
  canvas.drawBitmap(x, y, bitmap, w, h, c);
  }
  void beginFrame(int rotation) 
  {
    canvas.setRotation(rotation & 0x03);  // 限定 0~3
    canvas.fillScreen(1);                 // 白底
  }

  void endFrame() {
    memcpy(DisplayService::buffer(), canvas.getBuffer(), 2756);
    DisplayService::flush();
  }

  void clear(Color color)                    { canvas.fillScreen(toGfxColor(color)); }
  void setTextColor(Color color)             { canvas.setTextColor(toGfxColor(color)); }
  void setFont(const GFXfont* font)          { canvas.setFont(font); }
  void setCursor(int x, int y)               { canvas.setCursor(x, y); }
  void print(const char* str)                { canvas.print(str); }
  void print(int num)                        { canvas.print(num); }
  void print(const String& str) 
  {
    canvas.print(str);
  }
  void drawRect(int x, int y, int w, int h, Color color)
                                             { canvas.drawRect(x, y, w, h, toGfxColor(color)); }
  void fillRect(int x, int y, int w, int h, Color color)
                                             { canvas.fillRect(x, y, w, h, toGfxColor(color)); }
  void fillCircle(int x, int y, int r, Color color)
                                             { canvas.fillCircle(x, y, r, toGfxColor(color)); }

  int width()  { return EPD_WIDTH; }
  int height() { return EPD_HEIGHT; }

  uint16_t utf8_decode(const char* s, int* next) {
  uint8_t c = (uint8_t)s[0];
  if (c < 0x80)            { *next = 1; return c; }
  if ((c & 0xE0) == 0xC0)  { *next = 2; return ((c & 0x1F) << 6) | (s[1] & 0x3F); }
  if ((c & 0xF0) == 0xE0)  { *next = 3; return ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F); }
  *next = 1;
  return 0;
}

int drawChineseChar(int x, int y, uint16_t codepoint, Color color) {
  const CnGlyph* g = findGlyph(codepoint);
  if (!g) return 0;

  uint16_t fg = toGfxColor(color);
  for (int row = 0; row < 12; row++) {
    uint16_t bits = (g->data[row * 2] << 8) | g->data[row * 2 + 1];
    for (int col = 0; col < 12; col++) {
      if (bits & (0x8000 >> col)) {
        canvas.drawPixel(x + col, y + row, fg);
      }
    }
  }
  return 12;
}

  void printUTF8(int x, int y, const char* s, Color color) {
    int cx = x;
    int i = 0;
    while (s[i]) {
      int next = 0;
      uint16_t cp = utf8_decode(s + i, &next);
      i += next;

      if (cp < 0x80) {
        // ASCII：用 GFX 内置字体
        canvas.setTextColor(toGfxColor(color));
        canvas.setCursor(cx, y + 3);   // 基线对齐到 12px 行高
        canvas.write((char)cp);
        cx += 6;
      } else {
        int w = drawChineseChar(cx, y, cp, color);
        if (w == 0) {
          // 缺字：画一个空心方框提示
          canvas.drawRect(cx, y, 12, 12, toGfxColor(color));
          w = 12;
        }
        cx += w;
      }
    }
  }

}