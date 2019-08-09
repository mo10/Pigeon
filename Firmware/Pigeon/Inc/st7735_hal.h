#ifndef __ST7735_H__
#define __ST7735_H__

#include "fonts.h"
#include "main.h"
#include <stdbool.h>

/* 复位 */
#define ST7735_RES_Pin       LCD_RST_Pin
#define ST7735_RES_GPIO_Port LCD_RST_GPIO_Port
/* 片选 */
#define ST7735_CS_Pin        SPI1_CS_Pin
#define ST7735_CS_GPIO_Port  SPI1_CS_GPIO_Port
/* 模式 */
#define ST7735_DC_Pin        LCD_DC_Pin
#define ST7735_DC_GPIO_Port  LCD_DC_GPIO_Port


#define ST7735_MADCTL_MY  0x80
#define ST7735_MADCTL_MX  0x40
#define ST7735_MADCTL_MV  0x20
#define ST7735_MADCTL_ML  0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH  0x04

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Color definitions
#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0xFF07
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

#define DELAY 0x80

typedef struct ST7735_cmdBuf {
  uint8_t pack;
  uint8_t command;   // ST7735 command byte
  uint8_t delay;     // ms delay after
  uint8_t len;       // length of parameter data
  uint8_t data[16];  // parameter data
}ST7735_CMD_BUFTypeDef;

typedef struct ST7735_Profile{
    uint8_t XStart;
    uint8_t YStart;
    uint8_t Width;
    uint8_t Height;
    uint8_t Rotate;
}ST7735_ProfileTypDef;

uint8_t ST7735_IsReady();
void ST7735_SetProfile(uint8_t idx);
void ST7735_Init(SPI_HandleTypeDef * spi);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
uint8_t ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ST7735_InvertColors(bool invert);
void ST7735_print(const char* str,FontDef font, uint16_t color, uint16_t bgcolor);
void ST7735_println(const char* str,FontDef font, uint16_t color, uint16_t bgcolor);

void ST7735_SetDrawArea(uint16_t startX, uint16_t startY,uint16_t endX, uint16_t endY);
void ST7735_WriteData(uint8_t* buff, uint32_t buff_size);

void ST7735_Select();
void ST7735_Unselect();
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

void ST7735_DrawRLE(uint16_t *colorTab, uint16_t *data, uint16_t len);
#endif // __ST7735_H__
