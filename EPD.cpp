#include "EPD.h"
#include <SPI.h>

void EPD_Init()
{
  pinMode(EPD_BUSY_PIN, INPUT);
  pinMode(EPD_RST_PIN,  OUTPUT);
  pinMode(EPD_DC_PIN,   OUTPUT);
  pinMode(EPD_CS_PIN,   OUTPUT);

  digitalWrite(EPD_CS_PIN, HIGH);
  digitalWrite(EPD_DC_PIN, LOW);

  // RP2040 的 SPI0 引脚重映射
  SPI.setSCK(EPD_SCK_PIN);
  SPI.setTX(EPD_SDA_PIN);
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  digitalWrite(EPD_RST_PIN, LOW);
  delay(100);
  digitalWrite(EPD_RST_PIN, HIGH);
  delay(100);
}

void hardwareSpi(uint8_t data)
{
  SPI.transfer(data);
}

void sendCmd(uint8_t cmd)
{
  digitalWrite(EPD_DC_PIN, LOW);
  digitalWrite(EPD_CS_PIN, LOW);
  hardwareSpi(cmd);
  digitalWrite(EPD_CS_PIN, HIGH);
}

void sendData(uint8_t data)
{
  digitalWrite(EPD_DC_PIN, HIGH);
  digitalWrite(EPD_CS_PIN, LOW);
  hardwareSpi(data);
  digitalWrite(EPD_CS_PIN, HIGH);
}
