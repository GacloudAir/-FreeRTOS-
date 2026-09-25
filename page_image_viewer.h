#pragma once
#include "page.h"

class PageImageViewer : public Page {
public:
  bool open(const char* path);
  bool customRender() const override { return grayMode; }
  void onDraw(GraphicsAPI::Color accent) override;
  void onInput(InputService::Button btn) override;
  bool isGrayMode() const { return grayMode; }
  void onLongPress(InputService::Button btn) override;
  const char* name() const override { return "ImageViewer"; }

private:
  static const int IMG_W = 212;
  static const int IMG_H = 104;
  static const int IMG_BYTES = ((IMG_W + 7) / 8) * IMG_H;   // 2808
  static const int IMG_BYTES_GRAY = 212 * 104;   // 8-bit 灰度缓冲区
  uint8_t grayBitmap[IMG_BYTES_GRAY];
  bool grayMode = false;
  uint8_t bitmap[IMG_BYTES];
  char    title[24] = {0};
  bool    loaded = false;
  char  currentPath[80] = {0};

};

extern PageImageViewer imageViewerPage;