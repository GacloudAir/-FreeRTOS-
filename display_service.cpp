#include "display_service.h"
#include <string.h>

namespace {
  uint8_t epdBuffer[2756];
}

namespace DisplayService {

  void init() {
    EPD_Init();
    memset(epdBuffer, 0xFF, sizeof(epdBuffer));
  }

  uint8_t* buffer() { return epdBuffer; }

  void flush() {
    EPaper_WriteBWImage(epdBuffer, 0, 0, EPD_WIDTH, EPD_HEIGHT, false, false);
  }

  int width()  { return EPD_WIDTH; }   // 104
  int height() { return EPD_HEIGHT; }  // 212
  void flushGray(uint8_t* logicalGray) 
  {
    // 尺寸：物理 104×212，逻辑 212×104
    static uint8_t physicalGray[104 * 212];
    static uint8_t greyBuf[104 * 212 / 4];   // 2-bit 打包后 5512 字节

    // 1. 逻辑 → 物理坐标转换
    for (int py = 0; py < 212; py++) {
      for (int px = 0; px < 104; px++) {
        int lx = 211 - py;
        int ly = px;
        physicalGray[py * 104 + px] = logicalGray[ly * 212 + lx];
      }
    }

    // 2. 8-bit 灰度 → 2-bit（4 级）
    for (int i = 0; i < 104 * 212; i++) {
      uint8_t v = physicalGray[i];
      uint8_t level;
      if      (v < 85)  level = 0;   // 黑
      else if (v < 170) level = 1;   // 灰（用 g1 深灰，不用 g2 浅灰）
      else              level = 3;   // 白

      int byteIdx = i / 4;
      int shift = (3 - (i % 4)) * 2;
      greyBuf[byteIdx] &= ~(0x03 << shift);
      greyBuf[byteIdx] |= (level << shift);
    }

    // 3. 推送到屏幕
    EPaper_WriteGreyImage(greyBuf, 0, 0, 104, 212, false, false);
  }
}