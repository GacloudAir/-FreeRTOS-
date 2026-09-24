#pragma once
#include "app.h"

namespace AppRegistry {
  int count();
  const App& get(int index);
  const App* find(const char* id);
}