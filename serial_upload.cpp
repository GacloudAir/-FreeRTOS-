#include "serial_upload.h"
#include "storage_service.h"
#include <LittleFS.h>
#include "time_service.h" 

namespace {
  String readLine(unsigned long timeoutMs = 500) {
    String line;
    unsigned long start = millis();
    while (millis() - start < timeoutMs) {
      while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') return line;
        if (c != '\r') line += c;
      }
      delay(1);
    }
    return line;
  }
}

namespace SerialUpload {

  void init() {
    while (Serial.available()) Serial.read();
  }

  bool poll() {
    if (!Serial.available()) return false;

    String cmd = readLine();
    if (cmd.length() == 0) return false;

    if (cmd == "PING") {
      Serial.println("PONG");
      return false;
    }

    if (cmd == "REBOOT") {
      Serial.println("OK");
      Serial.flush();
      delay(200);
      rp2040.reboot();
      return false;
    }

    if (cmd == "PING") {
      Serial.println("PONG");
      return false;
    }
    
    if (cmd.startsWith("SET tz ")) 
    {
      int32_t tz = cmd.substring(7).toInt();
      TimeService::setTimezoneOffset(tz);
      Serial.println("OK");
      return false;
    }

    if (cmd.startsWith("LS ")) 
    {
      String path = cmd.substring(3);
      File dir = LittleFS.open(path.c_str(), "r");
      if (!dir || !dir.isDirectory()) {
        Serial.println("ERR not a dir");
        return false;
      }
      File f = dir.openNextFile();
      while (f) {
        Serial.print(f.name());
        Serial.print(" ");
        Serial.println(f.size());
        f = dir.openNextFile();
      }
      Serial.println("END");
      return false;
    }

    if (cmd.startsWith("RM ")) {
      String path = cmd.substring(3);
      if (LittleFS.remove(path.c_str())) Serial.println("OK");
      else                                Serial.println("ERR remove failed");
      return false;
    }

    if (cmd.startsWith("PUT ")) {
      // 格式: PUT <path> <size>
      int sp1 = cmd.indexOf(' ', 4);
      if (sp1 < 0) { Serial.println("ERR bad cmd"); return false; }

      String path = cmd.substring(4, sp1);
      int size = cmd.substring(sp1 + 1).toInt();
      if (size <= 0 || path.length() == 0) {
        Serial.println("ERR bad args");
        return false;
      }

      File f = LittleFS.open(path.c_str(), "w");
      if (!f) { Serial.println("ERR open failed"); return false; }

      Serial.println("READY");
      Serial.flush();

      // 接收字节，带超时
      unsigned long lastByte = millis();
      int received = 0;
      uint8_t buf[512];

      while (received < size) {
        int avail = Serial.available();
        if (avail > 0) {
          int want = min(avail, min((int)sizeof(buf), size - received));
          int got = Serial.readBytes(buf, want);
          f.write(buf, got);
          received += got;
          lastByte = millis();
        } else {
          if (millis() - lastByte > 5000) {
            // 超时
            break;
          }
          delay(1);
        }
      }

      f.close();

      if (received == size) {
        Serial.println("OK");
      } else {
        Serial.print("ERR incomplete ");
        Serial.println(received);
        LittleFS.remove(path.c_str());
      }
      return true;
    }
    Serial.println("ERR unknown cmd");
    return false;
  }
}