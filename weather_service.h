#pragma once
#include <Arduino.h>

namespace WeatherService {

  struct Data {
    float         temp;
    int           wcode;
    int           humid;
    bool          valid;
    unsigned long timestamp;
  };

  const Data& get();
  bool        fetch();                            // 需 WiFi 已连接
  bool        isFresh(unsigned long maxAgeMs);
}