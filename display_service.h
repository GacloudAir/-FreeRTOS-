#pragma once
#include "EPD.h"
#include "EPaper_213_Driver.h"

namespace DisplayService {
  void flushGray(uint8_t* grayBuffer);   // 传输灰度缓冲区到屏幕
  void init();
  uint8_t* buffer();
  void flush();
  int width();
  int height();

  // 空闲超过 idleMs 就把墨水屏断电（省电）；下次刷新会自动重新上电。
  void sleepIfIdle(unsigned long idleMs);
}