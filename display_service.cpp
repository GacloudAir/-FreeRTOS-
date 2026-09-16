#include "display_service.h"
#include <SPI.h>

// 在这里定义 display 对象（对应头文件里的 extern）
GxEPD2_3C<GxEPD2_270c, GxEPD2_270c::HEIGHT> display(
    GxEPD2_270c(/*CS=*/5, /*DC=*/3, /*RST=*/2, /*BUSY=*/10));

namespace DisplayService {

  void init() {
    SPI.setSCK(6);
    SPI.setTX(7);
    SPI.begin();
    display.init(115200, true, 2, false);
  }

  void beginFrame(int rotation) {
    display.setRotation(rotation);
    display.setFullWindow();
    display.firstPage();
  }

  void endFrame() {
    while (display.nextPage());
    display.hibernate();
  }
}