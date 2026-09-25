#include "app_registry.h"
#include "app_about.h"
#include "app_weather.h"
#include <string.h>
#include "app_clock.h" 

namespace {
  const App* list[] = {
    &clockApp,
    &aboutApp,
    &weatherApp,
  };
  const int listCount = sizeof(list) / sizeof(App*);
}

namespace AppRegistry {
  int count() { return listCount; }
  const App& get(int index) { return *list[index]; }
  const App* find(const char* id) {
    for (int i = 0; i < listCount; i++) {
      if (strcmp(list[i]->id, id) == 0) return list[i];
    }
    return nullptr;
  }
}