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
    current.version  = 4;
    current.page     = 0;
    current.rotation = 3;   // 默认横屏
  }

  void load() 
  {
    resetToDefaults();
    if (!StorageService::exists(SETTINGS_PATH)) 
    {
      Serial.println("No settings file, using defaults.");
      return;
    }

    String raw = StorageService::read(SETTINGS_PATH);
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, raw);
    if (err) 
    {
      Serial.print("Settings parse failed: ");
      Serial.println(err.c_str());
      return;
    }

    int storedVersion = doc["version"] | 0;
    current.page = doc["page"] | current.page;

    // v3 之前的版本没有横屏默认值，强制设置为 3
    if (storedVersion >= 4) 
      {
        current.rotation = doc["rotation"] | current.rotation;
      } else 
      {
        current.rotation = 3;
        Serial.println("Settings migrated to v4, rotation forced to 3");
      }

    current.version = 4;
    save();   // 保存迁移后的配置
    Serial.println("Settings loaded.");
  }

  bool save() 
  {
    JsonDocument doc;
    doc["version"]  = current.version;
    doc["page"]     = current.page;
    doc["rotation"] = current.rotation;

    String out;
    serializeJson(doc, out);
    bool ok = StorageService::write(SETTINGS_PATH, out);
    Serial.println(ok ? "Settings saved." : "Settings save FAILED.");
    return ok;
  }
}