#include "storage_service.h"
#include <LittleFS.h>

namespace StorageService {

  bool begin() {
    // 第一次尝试挂载
    if (LittleFS.begin()) {
      Serial.println("LittleFS mounted.");
      return true;
    }

    Serial.println("LittleFS mount failed, formatting...");
    LittleFS.format();
    delay(100);

    // 格式化后重试最多 3 次
    for (int i = 0; i < 3; i++) {
      if (LittleFS.begin()) {
        Serial.println("LittleFS mounted after format.");
        return true;
      }
      Serial.print("Retry mount ");
      Serial.println(i + 1);
      delay(200);
    }

    Serial.println("LittleFS mount failed after format!");
    return false;
  }

  String read(const char* path) {
    File f = LittleFS.open(path, "r");
    if (!f) return "";
    String s = f.readString();
    f.close();
    return s;
  }

  bool write(const char* path, const String& content) {
    File f = LittleFS.open(path, "w");
    if (!f) return false;
    f.print(content);
    f.close();
    return true;
  }

  bool exists(const char* path) {
    return LittleFS.exists(path);
  }

  bool remove(const char* path) {
    return LittleFS.remove(path);
  }

  int listDir(const char* path, FileInfo* out, int maxCount) {
    File dir = LittleFS.open(path, "r");
    if (!dir || !dir.isDirectory()) return 0;

    int count = 0;
    File f = dir.openNextFile();
    while (f && count < maxCount) {
      out[count].name = String(f.name());
      out[count].size = f.size();
      count++;
      f = dir.openNextFile();
    }
    return count;
  }

}