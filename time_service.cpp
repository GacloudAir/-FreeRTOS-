#include "time_service.h"
#include "network_service.h"
#include <WiFiEspAT.h>
#include <string.h>

namespace {
  bool synced = false;
  time_t baseEpoch = 0;
  unsigned long baseMillis = 0;
  int32_t tzOffset = 8 * 3600;   // UTC+8

  unsigned long lastSyncAttempt = 0;
  const unsigned long SYNC_INTERVAL  = 3600UL * 1000;
  const unsigned long RETRY_INTERVAL = 30UL * 1000;

  // 手动解析 Unix epoch（避免依赖 timegm）
  time_t buildEpoch(int year, int month, int day,
                    int hour, int min, int sec) {
    static const int daysInMonth[] =
      {31,28,31,30,31,30,31,31,30,31,30,31};

    long days = 0;
    for (int y = 1970; y < year; y++) {
      bool leap = ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0);
      days += leap ? 366 : 365;
    }
    bool leap = ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
    for (int m = 0; m < month - 1; m++) {
      days += daysInMonth[m];
      if (m == 1 && leap) days++;
    }
    days += day - 1;

    return (time_t)days * 86400
         + hour * 3600 + min * 60 + sec;
  }

  int monthFromStr(const char* s) {
    static const char* names[] =
      {"Jan","Feb","Mar","Apr","May","Jun",
       "Jul","Aug","Sep","Oct","Nov","Dec"};
    for (int i = 0; i < 12; i++) {
      if (strncmp(s, names[i], 3) == 0) return i + 1;
    }
    return 0;
  }

  bool doHttpSync() {
    // 用百度，响应头里含标准 Date 字段
    WiFiClient client;
    if (!client.connect("www.baidu.com", 80)) return false;

    client.print("HEAD / HTTP/1.0\r\n");
    client.print("Host: www.baidu.com\r\n");
    client.print("User-Agent: curl/7.68.0\r\n");
    client.print("\r\n");

    unsigned long t0 = millis();
    while (client.available() == 0) {
      if (millis() - t0 > 10000) {
        client.stop();
        return false;
      }
      delay(10);
    }

    // 只读前 512 字节，Date 一定在前面
    String head = "";
    t0 = millis();
    while (client.connected() || client.available()) {
      if (client.available()) {
        head += (char)client.read();
        if (head.length() >= 512) break;
        t0 = millis();
      } else if (millis() - t0 > 2000) {
        break;
      }
    }
    client.stop();

    // 找 "Date: Wed, 23 Sep 2026 09:04:00 GMT"
    int idx = head.indexOf("Date: ");
    if (idx < 0) return false;
    idx += 6;

    // 解析：Wed, 23 Sep 2026 09:04:00 GMT
    //       跳过 "Wed, " 或 "Wed "
    int dayStart = idx;
    while (head[dayStart] != ' ' && dayStart < idx + 5) dayStart++;
    while (head[dayStart] == ' ') dayStart++;

    int day = head.substring(dayStart, dayStart + 2).toInt();

    int monStart = head.indexOf(' ', dayStart) + 1;
    int month = monthFromStr(head.c_str() + monStart);
    if (month == 0) return false;

    int yearStart = head.indexOf(' ', monStart) + 1;
    int year = head.substring(yearStart, yearStart + 4).toInt();

    int timeStart = head.indexOf(' ', yearStart) + 1;
    int hour = head.substring(timeStart, timeStart + 2).toInt();
    int min  = head.substring(timeStart + 3, timeStart + 5).toInt();
    int sec  = head.substring(timeStart + 6, timeStart + 8).toInt();

    if (year < 2020 || year > 2100) return false;

    baseEpoch  = buildEpoch(year, month, day, hour, min, sec);
    baseMillis = millis();
    synced     = true;
    return true;
  }
}

namespace TimeService {

  void start() {
    baseEpoch = 0;
    baseMillis = 0;
    synced = false;
    lastSyncAttempt = 0;
  }

  void poll() {
    if (!NetworkService::isConnected()) return;

    unsigned long nowMs = millis();
    unsigned long interval = synced ? SYNC_INTERVAL : RETRY_INTERVAL;
    if (nowMs - lastSyncAttempt < interval) return;
    lastSyncAttempt = nowMs;

    if (!NetworkService::lockUart(5000)) return;
    doHttpSync();
    NetworkService::unlockUart();
  }

  bool isSynced() { return synced; }

  time_t now() {
    if (!synced) return 0;
    time_t elapsed = (millis() - baseMillis) / 1000;
    return baseEpoch + elapsed + tzOffset;
  }

  String formatTime() {
    if (!synced) return "--:--";
    time_t t = now();
    struct tm* tm = gmtime(&t);
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", tm->tm_hour, tm->tm_min);
    return String(buf);
  }

  String formatDate() {
    if (!synced) return "----/--/--";
    time_t t = now();
    struct tm* tm = gmtime(&t);
    char buf[16];
    snprintf(buf, sizeof(buf), "%04d/%02d/%02d",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    return String(buf);
  }

  String formatWeekday() {
    if (!synced) return "---";
    static const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    time_t t = now();
    struct tm* tm = gmtime(&t);
    return String(days[tm->tm_wday]);
  }

  void setTimezoneOffset(int32_t sec) { tzOffset = sec; }
  int32_t getTimezoneOffset() { return tzOffset; }
}