#include "display_service.h"
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>

// 一次性基准：开机时对同一帧内容只改帧率寄存器(0x30)，比较波形时长。
// 实测结论：0x25/0x13/0x4f 会让面板不再释放 BUSY（见 EPaper_refresh 的自愈逻辑），
// 0x39 是唯一可用值，所以此基准默认关闭，保留代码备查。
#define EPD_BENCH 0

namespace {
  uint8_t epdBuffer[2756];
  unsigned long lastFlushMs = 0;

  // 面板访问互斥：flush() 来自 taskUI，sleepIfIdle() 来自 taskBg。
  // 没有它就会出现"taskBg 正在掉电（PowerOff 内含 1.5s 延时、且已把 0x82/0x01
  // 改成掉电值），taskUI 同时在刷新"的竞态 —— 表现为自动刷新后白屏。
  SemaphoreHandle_t epdMutex = nullptr;
}

namespace DisplayService {

  void init() {
    if (!epdMutex) epdMutex = xSemaphoreCreateMutex();
    EPD_Init();
    memset(epdBuffer, 0xFF, sizeof(epdBuffer));

#if EPD_BENCH
    // 同一帧内容，只换 0x30 的值，比较 busy 时间，从而反推帧率。
    const uint8_t kPfs[] = { 0x39, EPD_LPRD_50HZ, EPD_LPRD_100HZ, EPD_LPRD_25HZ };
    for (uint8_t i = 0; i < sizeof(kPfs); i++) {
      Serial.print("[epd] BENCH 0x30=0x");
      Serial.println(kPfs[i], HEX);
      EPaper_SetFrameRate(kPfs[i]);
      EPaper_WriteBWImage(epdBuffer, 0, 0, EPD_WIDTH, EPD_HEIGHT, false, false);
      lastFlushMs = millis();
    }
    EPaper_SetFrameRate(EPD_PFS_DEFAULT);
    Serial.println("[epd] BENCH done, back to default");
#endif
  }

  uint8_t* buffer() { return epdBuffer; }

  void flush() {
    if (epdMutex) xSemaphoreTake(epdMutex, portMAX_DELAY);
    const unsigned long t0 = millis();
    EPaper_WriteBWImage(epdBuffer, 0, 0, EPD_WIDTH, EPD_HEIGHT, false, false);
    lastFlushMs = millis();
    Serial.print("[epd] flush total ");
    Serial.print(lastFlushMs - t0);
    Serial.println(" ms");
    if (epdMutex) xSemaphoreGive(epdMutex);
  }

  void sleepIfIdle(unsigned long idleMs) {
    if (!epdMutex) return;
    // 非阻塞获取：若 taskUI 正在刷新，本轮直接跳过。
    // 绝不能在刷新的中途掉电 —— 那正是 5 分钟自动刷新变白屏的原因。
    if (xSemaphoreTake(epdMutex, 0) != pdTRUE) return;
    if (EPaper_IsAwake() && (millis() - lastFlushMs) >= idleMs) {
      EPaper_Sleep();
    }
    xSemaphoreGive(epdMutex);
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
    if (epdMutex) xSemaphoreTake(epdMutex, portMAX_DELAY);
    EPaper_WriteGreyImage(greyBuf, 0, 0, 104, 212, false, false);
    lastFlushMs = millis();
    if (epdMutex) xSemaphoreGive(epdMutex);
  }
}