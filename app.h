#pragma once
#include "page.h"

struct App {
  const char* id;         // 唯一标识，如 "about"
  const char* name;       // 主菜单显示名，如 "About"
  Page* (*getPage)();     // 返回页面单例指针
};