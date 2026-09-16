#pragma once
#include <GxEPD2_3C.h>

// 对外暴露 display 对象，供 UI 层绘制使用
extern GxEPD2_3C<GxEPD2_270c, GxEPD2_270c::HEIGHT> display;

namespace DisplayService {
  void init();              // 初始化 SPI 和屏幕
  void beginFrame(int rotation);  // 开始一帧绘制
  void endFrame();          // 结束一帧并休眠
}