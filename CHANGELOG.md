# 更新日志

本项目遵循 [语义化版本](https://semver.org/lang/zh-CN/)。

## [未发布]

---

## [0.11.0] - 2026-09-28

### 新增
- **WiFi 按需开启**：不再保持长连接，改为"用一次断一次"
  - `NetworkService::connectOnce(timeout)`：同步连接，成功返回 true
  - `NetworkService::disconnect()`：断开并让 ESP8285 空闲
  - 平均电流从 ~80mA 降至 <25mA
- **天气数据缓存**（`weather_service.h/.cpp`）
  - `WeatherService::fetch()`：拉取 Open-Meteo 数据并缓存
  - `WeatherService::isFresh(maxAgeMs)`：判断数据是否过期
  - `WeatherService::get()`：读取缓存（含时间戳）
- **整点自动同步**：taskBg 在以下时机自动同步
  - 开机后 5 秒首次同步
  - 每小时整点（NTP + 天气）
  - 天气数据超过 55 分钟
  - 同步失败后 5 分钟重试

### 变更
- `time_service` 从"poll() 内部判断"改为"`syncNow()` 显式触发"
- `app_main_page` 和 `app_weather` 改为从 `WeatherService` 读取缓存，不再各自发 HTTP 请求
- `kernel.cpp` 移除 `NetworkService::start()` 调用（不再需要开机连接）
- Home 页天气信息来自缓存，无网络时显示 `Waiting weather...`

### 修复
- **开机自动刷新**：`taskUI` 启动后主动调用一次 `flushIfDirty()`，无需按键
- **联网后自动刷新**：`taskUI` 每 500ms 定期检查 `dirty`，配合信号量兜底
  - 修复了信号量丢失导致屏幕一直停在 `Waiting weather...` 的问题

## [未发布]

### 计划
- **M10**：系统设置完善（WiFi 凭据界面、时区配置、刷新模式差异化）
- **M11**：时钟应用增强（NTP 自动同步、闹钟）
- **M12**：更多应用（Todo / Calendar / RSS）
- **M13**：电源管理（深度休眠、电池监测）
- **M14**：OTA 更新
- **M15**：PCB 精简与亚克力外壳

## [0.10.0] - 2026-09-27

### 新增
- **Home 主页面应用**（`app_main_page.h/.cpp`）
  - 开机自动进入，10 分钟无操作自动返回
  - 左上角显示天气图标 + 温度 + 天气描述 + 湿度
  - 中间显示大字时间和日期
  - 长按 OK 进入主菜单
  - 天气每 55 分钟自动刷新（整点附近）
  - 时钟每 5 分钟自动刷新
- **天气图标**（`weather_icons.h`）
  - 6 个 16x16 1-bit 图标：晴、多云、雾、雨、雪、雷
  - WMO 天气代码 → 图标 / 英文描述映射函数
- `Page` 基类增加 `onTick()` 虚函数，供内核定期调用
- `PageManager::resetTo()` 接口：清空页面栈，只保留指定页面

## [0.9.0] - 2026-09-25

### 新增
- **网络时钟**：通过百度 HTTP `Date` 响应头同步时间，每 5 分钟自动刷新
- **时钟页**：大字时间显示 + 日期 + 星期
- **网页配置工具**（`PicoConfig2.html`）：通过 Web Serial 上传 WiFi 配置，支持断开、清空日志、清除配置
- **串口配置协议**：`PING` / `LS` / `RM` / `PUT` / `REBOOT` 命令
- **WiFi 配置持久化**：保存在 `/wifi.json`，首次启动无需硬编码

### 变更
- `serial_upload` 从 `Kernel::run()` 迁移到独立的 `taskSerial` 任务
- `network_service` 从硬编码 WiFi 改为读取 `/wifi.json`
- 时钟页刷新频率从每分钟改为每 5 分钟（减少墨水屏损耗）

### 修复
- 修复 FreeRTOS 启用后串口配置工具无响应的问题
- 修复 `poll()` 与 `fetchWeather()` 并发访问 UART 的竞态
- 修复 `network_service.cpp` 中 `switch` 语句的大括号不匹配

---

## [0.8.0] - 2026-09-24

### 新增
- **WiFi 支持**：通过 ESP8285 模块 + WiFiEspAT 实现联网
- **天气应用**：调用 Open-Meteo API 显示青岛黄岛区实时天气
- **UART 互斥量**：`lockUart()` / `unlockUart()` 串行化网络访问
- **应用注册表**：`AppRegistry` 动态管理应用列表

### 变更
- 天气数据源从 wttr.in 更换为 Open-Meteo（HTTP + JSON）
- 主菜单从 5 项扩展为 6 项（新增 Clock）

### 修复
- 修复 ESP8285 二次 DNS 解析失败（改用 IP 直连）
- 修复 `HTTPClient` 与 `WiFiEspAT` 类定义冲突

---

## [0.7.0] - 2026-09-23

### 新增
- **FreeRTOS 多任务**：`taskInput` / `taskUI` / `taskBg` 三任务
- **智能 / 阻塞刷新模式**：设置页可切换
- **`dirty` 标志**：延迟刷新机制

### 变更
- `Kernel::run()` 从超级循环改为 `vTaskDelay`
- 输入事件通过 FreeRTOS 队列传递
- 刷新请求通过二值信号量触发

### 修复
- 修复 `PageSettings` 的 vtable 未生成

---

## [0.6.0] - 2026-09-22

### 新增
- **灰度图片查看**：支持 8-bit 灰度 BMP，4 级灰阶显示
- **删除确认对话框**：长按 OK 删除文件前弹出确认
- **图片查看器**：1-bit BMP 全屏显示

### 变更
- 屏幕驱动从 GxEPD2 更换为厂商提供的 `EPaper_213_Driver`
- 画布从 Adafruit GFX 改为 `GFXcanvas1`

---

## [0.5.0] - 2026-09-21

### 新增
- **中文字库**：12×12 点阵，UTF-8 解码
- **文本查看器**：逐行 / 逐页滚动，支持中英混排
- **文件管理页**：2 列 × 3 行布局，长按翻页

### 变更
- 主菜单布局从 1 列改为 2 列

---

## [0.4.0] - 2026-09-20

### 新增
- **页面框架**：`Page` 基类 + `PageManager` 页面栈
- **长短按事件**：`EVT_SHORT` / `EVT_LONG`
- **主菜单 / 设置页**

### 变更
- `kernel.cpp` 从直接绘制改为页面路由

---

## [0.3.0] - 2026-09-19

### 新增
- **LittleFS 存储**：设置持久化到 `/settings.json`
- **`StorageService`**：文件读写封装
- **`Settings`**：版本化配置加载 / 保存

### 修复
- 修复 Flash Size 未启用 FS 导致挂载失败

---

## [0.2.0] - 2026-09-18

### 新增
- **服务抽象**：`display_service` / `input_service` / `graphics_API`
- **三按键输入**：GPIO 11/12/13
- **显示模式切换**：黑白红 / 纯黑白

---

## [0.1.0] - 2026-09-17

### 新增
- 项目初始化
- **墨水屏点亮**：2.13 寸黑白屏，104×212
- **接线确认**：RST=2, DC=3, CS=5, SCK=6, SDA=7, BUSY=10
- **SPI 重映射**：`SPI.setSCK(6); SPI.setTX(7);`

---

## 里程碑对照

| 版本 | 里程碑 | 内容 |
|------|--------|------|
| v0.1 | M1 | 最小内核、屏幕点亮 |
| v0.2 | M2 | 服务抽象 |
| v0.3 | M3 | 存储与设置 |
| v0.4 | M4 | 页面框架 |
| v0.5 | M5–M6a | 文件管理 + 中文文本 |
| v0.6 | M6b | 图片显示 |
| v0.7 | M7 | FreeRTOS |
| v0.8 | M8–M9 | 应用注册表 + WiFi 天气 |
| v0.9 | M9.5 | 网络时钟 + 网页配置工具 |
| 未来 | M10–M15 | 见「未发布」 |