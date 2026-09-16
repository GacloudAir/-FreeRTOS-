#include "kernel.h"
#include "input_service.h"
#include "graphics_API.h"
#include "display_service.h"
#include <Arduino.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// ---- 应用状态 ----
static int  currentPage = 0;
static bool colorMode   = true;
static const int rotation = 3;

// ---- 绘制一帧 ----
static void redraw() {
  GraphicsAPI::beginFrame(rotation);
  GraphicsAPI::clear(GraphicsAPI::WHITE);
  GraphicsAPI::setTextColor(GraphicsAPI::BLACK);
  GraphicsAPI::setFont(&FreeMonoBold9pt7b);

  GraphicsAPI::setCursor(20, 30);
  GraphicsAPI::print("Page: ");
  GraphicsAPI::print(currentPage);

  GraphicsAPI::setCursor(20, 60);
  GraphicsAPI::print("Mode: ");
  GraphicsAPI::print(colorMode ? "BWR" : "BW");

  GraphicsAPI::Color accent = colorMode ? GraphicsAPI::RED : GraphicsAPI::BLACK;
  GraphicsAPI::drawRect(20, 100, 100, 80, accent);
  GraphicsAPI::fillCircle(200, 140, 30, accent);

  GraphicsAPI::endFrame();
}

// ---- 内核初始化 ----
void Kernel::init() {
  Serial.begin(115200);
  Serial.println("System booting...");

  DisplayService::init();   // 显示服务已在 graphics_API 内部使用，但初始化仍需调用
  InputService::init();

  redraw();
  Serial.println("System ready.");
}

// ---- 内核主循环 ----
void Kernel::run() {
  InputService::Button btn = InputService::poll();

  if (btn == InputService::BTN_UP) {
    currentPage++;
    redraw();
  }
  if (btn == InputService::BTN_DOWN) {
    currentPage--;
    redraw();
  }
  if (btn == InputService::BTN_OK) {
    colorMode = !colorMode;
    redraw();
  }

  delay(20);
}