#pragma once
#include <stdint.h>

// 读取 1-bit 未压缩 BMP 到 dst（尺寸必须为 ((dstW+7)/8) * dstH 字节）
// BMP 位序：位1=白、位0=黑；dst 位序：位1=黑、位0=白（已反转）
// 返回 true 表示成功
bool bmp_load_1bit(const char* path, uint8_t* dst, int dstW, int dstH);
// 读取 8-bit 灰度 BMP 到 dst（dst 大小应为 dstW * dstH 字节）
bool bmp_load_8bit_gray(const char* path, uint8_t* dst, int dstW, int dstH);