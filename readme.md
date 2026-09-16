# RP2040 墨水屏桌面系统

## 项目简介
基于 RP2040 + 2.7 寸三色墨水屏（黑白红）的嵌入式桌面显示系统。
目标：构建一个具备文件管理、设置调整、页面切换和扩展能力的轻量级固件框架。

## 硬件清单
- RP2040 开发板（Raspberry Pi Pico / Pico W / 兼容板）
- 微雪 2.7 寸三色墨水屏（176×264，黑白红）
- 微雪墨水屏驱动板 v1.3（24P FPC 接口）
- 轻触按键 ×3
- 杜邦线、USB 数据线

## 接线定义
### 墨水屏驱动板 → RP2040
| 驱动板 | RP2040 | 说明 |
|--------|--------|------|
| 3V3    | 3.3V   | 供电，禁止接 5V |
| GND    | GND    | 共地 |
| RST    | GPIO 2 | 复位 |
| DC     | GPIO 3 | 数据/命令 |
| CS1    | GPIO 5 | 片选 |
| SCK    | GPIO 6 | SPI 时钟 |
| SDA    | GPIO 7 | SPI 数据 |
| BUSY   | GPIO 10| 忙状态 |

### 按键 → RP2040
| 功能 | GPIO | 另一端 |
|------|------|--------|
| 上翻 | GPIO 11 | GND |
| 下翻 | GPIO 12 | GND |
| 确认 | GPIO 13 | GND |

按键使用内部上拉，按下为低电平。

## 软件依赖
- Arduino IDE
- Earle Philhower `arduino-pico` 核心
- 开发板选择：Raspberry Pi Pico
- 库：
  - GxEPD2
  - Adafruit GFX Library
  - U8g2_for_Adafruit_GFX（中文显示）
  - LittleFS（后续里程碑使用）

## 里程碑记录

### 里程碑 1：最小内核
- 单文件实现按键轮询、屏幕刷新、模式切换。
- 验证 RP2040 与墨水屏基础链路。

### 里程碑 2：服务抽象模块化
将单文件拆分为多个模块，明确职责边界：
- `display_service`：只负责 SPI 配置、屏幕初始化、`firstPage()/nextPage()` 和休眠。
- `input_service`：只负责按键消抖和事件输出，返回 `BTN_UP / BTN_DOWN / BTN_OK`。
- `0916.ino`：仅保留 `setup()` 和 `loop()`，调用内核。

### 里程碑 2.1：逻辑内核与图形内核分离
进一步拆分为 `kernel` 和 `graphics_API`：
- `kernel`：
  - 维护应用状态：`currentPage`、`colorMode`、`rotation`。
  - 处理按键事件并决定何时重绘。
  - 不直接调用 `SPI`、`pinMode`、`display.init()`。
- `graphics_API`：
  - 提供颜色枚举 `BLACK / WHITE / RED`。
  - 提供 `beginFrame`、`endFrame`、`clear`、`setTextColor`、`setFont`、`setCursor`、`print`、`drawRect`、`fillCircle` 等绘图 API。
  - 内部将 `GraphicsAPI::Color` 映射为 `GxEPD_BLACK / WHITE / RED`。
  - 隔离 GxEPD2，方便未来更换屏幕或驱动。

### 里程碑 2.2：中文显示与多页测试（进行中）
- 引入 `U8g2_for_Adafruit_GFX`，在保留原有英文 `FreeMonoBold9pt7b` 字体的同时支持中文。
- 使用 `u8g2_fontmaker` 生成细体中文字库，仅包含项目所需汉字与符号，控制 Flash 占用。
- `graphics_API` 增加：
  - `u8g2Fonts` 对象
  - `setU8g2Font(const uint8_t* font)`
  - `printUTF8(const char* str)`
- `kernel` 设计多页测试：
  - 第 1 页：ASCII 可见符号、数字、大小写字母。
  - 第 2 页：常用汉字（一二三四五六七八九十、上下左右前后内外、天地日月星辰山海等）。
  - 第 3 页及以后：预留更多符号、标点和扩展汉字。
- 翻页逻辑：
  - 上键：下一页（循环）。
  - 下键：上一页（循环）。
  - OK 键：保留黑白红 / 纯黑白显示模式切换。

## 当前功能
- 上/下键翻页，OK 键切换显示模式。
- 屏幕旋转 `rotation = 3`（可按需改为 0/1/2/3）。
- 每次刷新为全屏刷新，刷新后进入休眠。
- 英文使用 `FreeMonoBold9pt7b`，中文使用自定义 U8g2 字库。

## 已知限制
- 三色墨水屏仅支持全屏刷新，每次刷新约 15 秒，无法做实时动画。
- FPC 排线脆弱，插拔需小心，建议固定根部。
- 墨水屏工作电压 3.3V，禁止接 5V。
- 中文显示依赖自定义字库，字库越大占用 Flash 越多。

## 编译与上传
1. 安装 Earle Philhower `arduino-pico` 核心。
2. 开发板选择 `Raspberry Pi Pico`。
3. 安装 GxEPD2、Adafruit GFX、U8g2_for_Adafruit_GFX。
4. 将本项目文件夹在 Arduino IDE 中打开，编译上传。
5. 串口波特率 115200，可查看启动日志。

## 后续计划
- 里程碑 3：存储与设置
  - 使用 LittleFS 保存 `settings.json`。
  - 持久化 `currentPage`、`colorMode`、`rotation`。
- 里程碑 4：页面框架
  - 页面基类、页面栈、菜单导航。
- 里程碑 5：FreeRTOS 与扩展能力
  - 双核任务划分：UI 任务、逻辑任务、低功耗任务。
  - 应用注册表，支持时钟、天气、待办等扩展。

## 版本
- v0.2.0 里程碑 2：服务抽象、逻辑/图形内核分离、中文显示与多页测试设计。