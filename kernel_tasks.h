#pragma once

namespace KernelTasks {
  void begin();
  void requestRefresh();
  void requestSync();     // 请求 taskBg 立即重新同步时间+天气（非阻塞）
}