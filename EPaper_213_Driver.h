#ifndef __EPAPER_213_DRIVER_H
#define __EPAPER_213_DRIVER_H

#include <Arduino.h>
#include "global.h"
#include "EPD.h"


#define     EPD_WIDTH 104
#define     EPD_HEIGHT 212
#define     power_on_time 20
#define     full_refresh_time 7500

// #define SET_DC_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_11)
// #define	SET_CS_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_12)
// #define SET_DC_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_11)
// #define	SET_CS_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_12)
// #define	SET_RST_LOW GPIO_ResetBits(GPIOB, GPIO_Pin_1)
// #define	SET_RST_HIGH GPIO_SetBits(GPIOB, GPIO_Pin_1)
// #define GET_BUSY_STATUS GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)

void EPaper_WriteBWImage(const uint8_t* black,uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool invert, bool mirror_y);
void EPaper_WriteGreyImage(const uint8_t* Grey,uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool invert, bool mirror_y);

// ---- 面板生命周期 ----
// 原来每次 Write*Image 都要走一遍"复位 + 上电 + 重灌 8 组 LUT + 刷新 + 断电(delay 1.5s)"。
// 对需要频繁刷新的 UI 来说这是纯开销，所以改为：把面板保持上电，只在空闲时断电。
void EPaper_EnsureInit(void);   // 幂等：未初始化时才做复位+上电+LUT 重灌
void EPaper_Sleep(void);        // 断电，可重复调用
bool EPaper_IsAwake(void);

// 帧率寄存器 0x30 (PFS)。LUT 里的帧数不变，但每帧时长随帧率变化，
// 所以整个波形的 wall-clock 时间与帧率成反比。
// 厂商在 EPD.h 里给了三档：EPD_LPRD_25HZ(0x4f) / EPD_LPRD_50HZ(0x25) / EPD_LPRD_100HZ(0x13)。
// 驱动原本写死 0x39，实测波形 3893 ms（约 50Hz）。
#ifndef EPD_PFS_DEFAULT
#define EPD_PFS_DEFAULT 0x39
#endif
void EPaper_SetFrameRate(uint8_t v);

#endif