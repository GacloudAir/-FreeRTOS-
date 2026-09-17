#include "storage_service.h"
#include <LittleFS.h>

namespace StorageService {

  bool begin() {
    if (!LittleFS.begin()) {
      Serial.println("LittleFS mount failed, formatting...");
      LittleFS.format();
      if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed after format!");
        return false;
      }
    }
    Serial.println("LittleFS mounted.");
    return true;
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
}