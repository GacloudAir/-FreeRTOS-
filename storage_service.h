#pragma once
#include <Arduino.h>

namespace StorageService 
{
  bool begin();                       // 挂载 LittleFS，失败返回 false
  String read(const char* path);      // 读整个文件为字符串
  bool write(const char* path, const String& content);  // 覆盖写入
  bool exists(const char* path);
  bool remove(const char* path);
}