#pragma once
#include <stdint.h>

struct CnGlyph {
  uint16_t codepoint;   // Unicode 码点
  uint8_t  data[24];    // 12 行 × 2 字节，MSB 在左
};

extern const CnGlyph font_cn_table[];
extern const int font_cn_count;
