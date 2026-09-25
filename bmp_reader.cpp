#include "bmp_reader.h"
#include "storage_service.h"
#include <LittleFS.h>
#include <string.h>

namespace {
  const int OFF_DATA_OFFSET = 10;
  const int OFF_WIDTH       = 18;
  const int OFF_HEIGHT      = 22;
  const int OFF_BITCOUNT    = 28;
  const int OFF_COMPRESSION = 30;
  const int HEADER_SIZE     = 62;   // 14 + 40 + 8（调色板）
}

bool bmp_load_1bit(const char* path, uint8_t* dst, int dstW, int dstH) {
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  uint8_t header[HEADER_SIZE];
  if (f.read(header, HEADER_SIZE) != HEADER_SIZE) {
    f.close();
    return false;
  }

  if (header[0] != 'B' || header[1] != 'M') {
    f.close();
    return false;
  }

  uint32_t dataOffset = header[OFF_DATA_OFFSET]
                      | (header[OFF_DATA_OFFSET + 1] << 8)
                      | (header[OFF_DATA_OFFSET + 2] << 16)
                      | (header[OFF_DATA_OFFSET + 3] << 24);

  int32_t width  = header[OFF_WIDTH]
                 | (header[OFF_WIDTH + 1] << 8)
                 | (header[OFF_WIDTH + 2] << 16)
                 | (header[OFF_WIDTH + 3] << 24);

  int32_t height = header[OFF_HEIGHT]
                 | (header[OFF_HEIGHT + 1] << 8)
                 | (header[OFF_HEIGHT + 2] << 16)
                 | (header[OFF_HEIGHT + 3] << 24);

  uint16_t bitCount = header[OFF_BITCOUNT]
                    | (header[OFF_BITCOUNT + 1] << 8);

  uint32_t compression = header[OFF_COMPRESSION]
                       | (header[OFF_COMPRESSION + 1] << 8)
                       | (header[OFF_COMPRESSION + 2] << 16)
                       | (header[OFF_COMPRESSION + 3] << 24);

  if (bitCount != 1 || compression != 0) {
    f.close();
    return false;
  }
  if (width != dstW || abs(height) != dstH) {
    f.close();
    return false;
  }

  int absH = abs(height);
  int bmpRowBytes = ((width + 31) / 32) * 4;   // BMP 4 字节行填充
  int dstRowBytes = (dstW + 7) / 8;            // 目标无填充

  for (int y = 0; y < absH; y++) {
    // height > 0 表示 BMP 自下而上存储，需要翻转
    int srcY = (height > 0) ? (absH - 1 - y) : y;
    f.seek(dataOffset + srcY * bmpRowBytes);

    uint8_t rowBuf[64];
    if (f.read(rowBuf, bmpRowBytes) != (int)bmpRowBytes) {
      f.close();
      return false;
    }

    uint8_t* dstRow = dst + y * dstRowBytes;
    for (int i = 0; i < dstRowBytes; i++) {
      dstRow[i] = ~rowBuf[i];   // 反转位序
    }
  }

  f.close();
  return true;
}

bool bmp_load_8bit_gray(const char* path, uint8_t* dst, int dstW, int dstH) 
{
  File f = LittleFS.open(path, "r");
  if (!f) return false;

  // 读取 BMP 头部（54 字节 + 调色板最多 1024 字节，先读前 54 字节）
  uint8_t header[54];
  if (f.read(header, 54) != 54) { f.close(); return false; }

  if (header[0] != 'B' || header[1] != 'M') { f.close(); return false; }

  int32_t width  = header[18] | (header[19] << 8) | (header[20] << 16) | (header[21] << 24);
  int32_t height = header[22] | (header[23] << 8) | (header[24] << 16) | (header[25] << 24);
  uint16_t bitCount = header[28] | (header[29] << 8);
  uint32_t compression = header[30] | (header[31] << 8) | (header[32] << 16) | (header[33] << 24);
  uint32_t dataOffset = header[10] | (header[11] << 8) | (header[12] << 16) | (header[13] << 24);

  if (bitCount != 8 || compression != 0) {
    f.close();
    return false;
  }
  if (width != dstW || abs(height) != dstH) {
    f.close();
    return false;
  }

  int absH = abs(height);
  int rowBytes = ((width + 3) / 4) * 4;   // BMP 4 字节行对齐

  for (int y = 0; y < absH; y++) {
    int srcY = (height > 0) ? (absH - 1 - y) : y;   // height>0 表示自下而上
    f.seek(dataOffset + srcY * rowBytes);
    f.read(dst + y * dstW, dstW);   // 每行读 dstW 字节
  }

  f.close();
  return true;
}