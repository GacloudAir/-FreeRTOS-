#pragma once
#include <Arduino.h>

namespace StorageService {

  struct FileInfo {
    String name;
    size_t size;
  };

  bool   begin();
  String read(const char* path);
  bool   write(const char* path, const String& content);
  bool   exists(const char* path);
  bool   remove(const char* path);

  int    listDir(const char* path, FileInfo* out, int maxCount);
}