#include "settings.h"
#include "storage_service.h"
#include <ArduinoJson.h>   // 需在库管理器安装 ArduinoJson

namespace {
  const char* SETTINGS_PATH = "/settings.json";
  Settings::Data current;
}

namespace Settings 
{

  Data& get() { return current; }

  void resetToDefaults() 
  {
    current.version     = 5;
    current.page        = 0;
    current.rotation    = 3;
    current.refreshMode = REFRESH_SMART;   // 默认智能
  }

  void load() 
  {
    resetToDefaults();
    if (!StorageService::exists(SETTINGS_PATH)) {
      return;
    }

    String raw = StorageService::read(SETTINGS_PATH);
    JsonDocument doc;
    if (deserializeJson(doc, raw)) return;

    int storedVersion = doc["version"] | 0;
    current.page = doc["page"] | current.page;

    if (storedVersion >= 4) {
      current.rotation = doc["rotation"] | current.rotation;
    } else {
      current.rotation = 3;
    }
    if (storedVersion >= 5) {
      current.refreshMode = doc["refreshMode"] | current.refreshMode;
    }

    current.version = 5;
    save();
  }

  bool save() {
    JsonDocument doc;
    doc["version"]     = current.version;
    doc["page"]        = current.page;
    doc["rotation"]    = current.rotation;
    doc["refreshMode"] = current.refreshMode;

    String out;
    serializeJson(doc, out);
    return StorageService::write(SETTINGS_PATH, out);
  }
}