# RP2040 墨水屏终端

基于 **RP2040 + 2.13 寸黑白墨水屏 + FreeRTOS** 的桌面终端，支持文件管理、中文文本查看、图片显示、联网天气、网络时钟，并提供网页端串口配置工具。

## 功能特性

- **FreeRTOS 多任务**：输入、UI、网络、串口配置四任务并行，按键不阻塞刷新
- **页面框架**：主菜单 / 时钟 / 天气 / 设置 / 文件 / 关于，页面栈导航
- **长短按事件**：短按在页面内操作，长按翻页或返回
- **文件系统**：LittleFS，支持列出、查看、删除文件
- **文本查看器**：支持 UTF-8 中文，逐行 / 逐页滚动
- **图片查看器**：支持 1-bit 和 8-bit 灰度 BMP
- **联网天气**：ESP8285 + Open-Meteo，显示青岛黄岛区实时天气
- **网络时钟**：HTTP Date 头同步时间，每 5 分钟刷新
- **设置持久化**：屏幕旋转、刷新模式（阻塞 / 智能）
- **网页配置工具**：`PicoConfig2.html` 通过 Web Serial 上传 WiFi 配置
- **应用注册表**：新增应用只需添加两个文件

## 硬件清单

| 组件 | 型号 / 说明 |
|------|-------------|
| 主控 | 第三方 RP2040 W 开发板（板载 **ESP8285** WiFi 模块） |
| 屏幕 | 2.13 寸黑白墨水屏，104×212，24P FPC，**UC8151** 驱动 IC |
| 驱动板 | 微雪 2.7/2.13/2.9 通用墨水屏驱动板 **v1.3** |
| 按键 | 3 个轻触开关（上 / 下 / 确认） |
| 供电 | USB 5V（后续可加锂电池 + TP4056） |

> ⚠️ 本项目的 WiFi 功能**不依赖 Pico W 官方 CYW43439**，而是通过 AT 指令驱动板载 ESP8285。因此即使使用非官方 Pico W 也能联网。

## 接线表

### 墨水屏（微雪 v1.3 驱动板 → RP2040）

| 驱动板 | RP2040 |
|--------|--------|
| RST | GPIO 2 |
| DC | GPIO 3 |
| CS1 | GPIO 5 |
| SCK | GPIO 6 |
| SDA (MOSI) | GPIO 7 |
| BUSY | GPIO 10 |
| 3V3 | 3.3V |
| GND | GND |

### 按键（另一端接 GND）

| 功能 | RP2040 |
|------|--------|
| 上 | GPIO 11 |
| 下 | GPIO 12 |
| 确认 | GPIO 13 |

### ESP8285（板载，UART）

| ESP8285 | RP2040 |
|---------|--------|
| TX | GPIO 1（`ESP_RX_PIN`） |
| RX | GPIO 0（`ESP_TX_PIN`） |
| 波特率 | 115200 |

## 开发环境

- **Arduino IDE** 2.x
- **开发板核心**：[Earle Philhower arduino-pico](https://github.com/earlephilhower/arduino-pico)（**不是** Arduino Mbed OS）
- **开发板选择**：`Raspberry Pi Pico W`
- **Operating System**：`FreeRTOS SMP`
- **Flash Size**：`2MB (Sketch: 1MB, FS: 1MB)` 或更大
- **CPU Frequency**：133MHz

### 依赖库

| 库 | 用途 |
|----|------|
| [GxEPD2](https://github.com/ZinggJM/GxEPD2) | 墨水屏驱动（仅参考，本项目最终改用厂商驱动） |
| [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) | 画布与图形绘制 |
| [ArduinoJson](https://arduinojson.org/) | JSON 解析（设置、天气） |
| [WiFiEspAT](https://github.com/jandrassy/WiFiEspAT) | ESP8285 AT 指令封装 |

> 所有库通过 Arduino IDE 的「库管理器」安装即可。

## 编译与上传

1. 用 Arduino IDE 打开 `BW_213.ino`。
2. 按上文设置开发板、OS、Flash Size。
3. 首次上传时按住 **BOOTSEL** 进入 UF2 模式。
4. 后续可直接点击上传。

## 首次配置 WiFi

1. 烧录固件后，屏幕显示 `No WiFi cfg`。
2. USB 连接电脑。
3. 用 **Chrome / Edge** 打开 `PicoConfig2.html`。
4. 点击「连接设备」，选择 RP2040 的 COM 口。
5. 填入 WiFi 名称和密码，点击「保存并重启设备」。
6. 设备自动重启并连接 WiFi。

> 配置文件保存在 `/wifi.json`。清除配置可在网页工具中点「清除 WiFi 配置」。

## 使用说明

### 按键

| 操作 | 效果 |
|------|------|
| 短按上 / 下 | 页面内光标移动 |
| 长按上 / 下 | 翻页 |
| 短按 OK | 选中 / 打开 |
| 长按 OK | 返回上一页 |

### 主菜单

```
┌─────────────┬─────────────┐
│ Clock       │ Weather     │
├─────────────┼─────────────┤
│ Settings    │ Files       │
├─────────────┼─────────────┤
│ About       │ Exit        │
└─────────────┴─────────────┘
```

### 设置项

- **Rotation**：屏幕旋转 0/1/2/3（默认 3）
- **Refresh**：刷新模式
  - `Block`：每次按键立即刷新（慢但确定）
  - `Smart`：短按合并刷新，长按立即刷新（默认）

## 文件结构

```
BW_213/
├── BW_213.ino              主入口
├── kernel.h / .cpp         逻辑内核
├── kernel_tasks.h / .cpp   FreeRTOS 任务定义
├── page.h                  页面基类
├── page_manager.h / .cpp   页面栈
├── page_main_menu.h / .cpp 主菜单页
├── page_settings.h / .cpp  设置页
├── page_file_manager.h/.cpp 文件管理页
├── page_text_viewer.h/.cpp 文本查看器
├── page_image_viewer.h/.cpp 图片查看器
├── page_confirm_dialog.h/.cpp 删除确认
├── app.h                   应用结构体
├── app_registry.h / .cpp   应用注册表
├── app_about.h / .cpp      关于页
├── app_weather.h / .cpp    天气页
├── app_clock.h / .cpp      时钟页
├── display_service.h/.cpp  显示服务
├── graphics_API.h / .cpp   图形 API
├── input_service.h / .cpp  输入服务
├── storage_service.h/.cpp  LittleFS
├── settings.h / .cpp       设置持久化
├── network_service.h/.cpp  WiFi 状态机
├── time_service.h / .cpp   时间同步
├── serial_upload.h / .cpp  串口配置协议
├── EPD.h / .cpp            ESP8285 底层
├── EPaper_213_Driver.h/.cpp 墨水屏驱动
├── font_cn_12x12.h / .cpp  中文字库
├── layout.h                布局参数
├── global.h                全局定义
└── PicoConfig2.html        网页配置工具
```

## 当前进度

见 [CHANGELOG.md](CHANGELOG.md)。

## 许可证

MIT License，详见 [LICENSE](LICENSE)。