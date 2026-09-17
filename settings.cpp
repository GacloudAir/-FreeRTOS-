#include "settings.h"
#include "storage_service.h"
#include <ArduinoJson.h>   // 需在库管理器安装 ArduinoJson

namespace {
  const char* SETTINGS_PATH = "/settings.json";
  Settings::Data current;
}

namespace Settings {

  Data& get() { return current; }

  void resetToDefaults() {
    current.version   = 1;
    current.page      = 0;
    current.colorMode = 1;
    current.rotation  = 3;
  }

  void load() {
    resetToDefaults();   // 先填默认值，再尝试覆盖
    if (!StorageService::exists(SETTINGS_PATH)) {
      Serial.println("No settings file, using defaults.");
      return;
    }

    String raw = StorageService::read(SETTINGS_PATH);
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, raw);
    if (err) {
      Serial.print("Settings parse failed: ");
      Serial.println(err.c_str());
      return;
    }

    current.version   = doc["version"]   | current.version;
    current.page      = doc["page"]      | current.page;
    current.colorMode = doc["colorMode"] | current.colorMode;
    current.rotation  = doc["rotation"]  | current.rotation;

    Serial.println("Settings loaded.");
  }

  bool save() {
    JsonDocument doc;
    doc["version"]   = current.version;
    doc["page"]      = current.page;
    doc["colorMode"] = current.colorMode;
    doc["rotation"]  = current.rotation;

    String out;
    serializeJson(doc, out);
    bool ok = StorageService::write(SETTINGS_PATH, out);
    Serial.println(ok ? "Settings saved." : "Settings save FAILED.");
    return ok;
  }
}