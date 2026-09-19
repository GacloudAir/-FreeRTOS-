#pragma once
#include <Arduino.h>

namespace SerialUpload {
  void init();       // 清空缓冲
  bool poll();       // 每次 loop 调用，处理一个命令；返回 true 表示正在接收
}