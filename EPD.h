#ifndef __EPD_H__
#define __EPD_H__

#include "global.h"

#define DATA_MASK   0x0100
#define DCX_CMD     0x0000
#define DCX_DATA    0x0001

#define PSR         0x0000
#define PWR         0x0001
#define POF         0x0002
#define PFS         0x0003
#define PON         0x0004
#define BTST        0x0006
#define DRF         0x0012
#define DTM1        0x0010
#define DTM2        0x0013
#define DTM3        0x0015
#define DTM4        0x0016
#define DTMW        0x0014
#define LUT_KWVCOM  0x0020
#define LUT_KW      0x0022
#define LUT_FT      0x0026
#define LPRD        0x0030
#define TSE         0x0041
#define TSC         0x0040
#define CDI         0x0050
#define TRES        0x0061
#define GDS         0x0062
#define GBS         0x0063
#define GSS         0x0064
#define VDCS        0x0082
#define VBDS        0x0084
#define ROTP        0x00A2
#define LVSEL       0x00E4
#define DF          0x00DF
#define LPSEL       0x00E4

#define A2_Mode     0x02
#define GU4_Mode    0x01
#define GC4_Mode    0x00

#define EPD_LPRD_25HZ     0x4f
#define EPD_LPRD_50HZ     0x25
#define EPD_LPRD_100HZ    0x13

#define BYTE_COUNTER          14400
#define BYTE_SIGNL_COUNTER    7200

// ============ RP2040 引脚定义 ============
#define EPD_SCK_PIN       6
#define EPD_SDA_PIN       7
#define EPD_CS_PIN        5
#define EPD_RST_PIN       2
#define EPD_DC_PIN        3
#define EPD_BUSY_PIN      10

#define SET_CS_HIGH     digitalWrite(EPD_CS_PIN, HIGH)
#define SET_CS_LOW      digitalWrite(EPD_CS_PIN, LOW)

#define SET_DC_HIGH     digitalWrite(EPD_DC_PIN, HIGH)
#define SET_DC_LOW      digitalWrite(EPD_DC_PIN, LOW)

#define SET_RST_HIGH    digitalWrite(EPD_RST_PIN, HIGH)
#define SET_RST_LOW     digitalWrite(EPD_RST_PIN, LOW)

#define EPD_BUSY_VAL    digitalRead(EPD_BUSY_PIN)

void EPD_Init();
void sendCmd(uint8_t cmd);
void sendData(uint8_t data);

#endif