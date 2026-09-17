#pragma once
#include <Arduino.h>

namespace Settings {

  struct Data {
    int  version;        // 配置版本号，便于未来迁移
    int  page;
    int  colorMode;      // 1 = 黑白红，0 = 黑白
    int  rotation;       // 0~3
  };

  Data& get();                        // 获取当前设置（可读可写）
  void  load();                       // 从 /settings.json 读取
  bool  save();                       // 写入 /settings.json
  void  resetToDefaults();            // 恢复出厂设置
}