#ifndef __USB_CMD_H__
#define __USB_CMD_H__

#include "main.h"
#include "st7735.h"
#include "usbd_customhid.h"

#define CMD_BUF_HEAD  0x22
#define CMD_BUF_END   0x33

typedef enum
{
  USB_CMD_INVERTCOLOR = 0,
  USB_CMD_STOP,
  USB_CMD_SET_BACKLIGHT,
  USB_CMD_SET_SCREENCOLOR,
  USB_CMD_DRAWIMAGE,
  USB_CMD_PRINTTEXT,
  USB_CMD_FILLRECT,
  USB_CMD_TOBOOTLOADER
}
USB_CMDTypeDef;

typedef enum
{
  USB_CMD_OK = 0,
  USB_CMD_FAIL
}
USB_CMD_RetypeDef; 

typedef struct USB_CMD_BUF
{
  uint8_t head;
  uint8_t cmd;
  uint16_t length;
  uint8_t data[4];
  uint8_t end;
}__attribute__ ((packed)) USB_CMD_BUFTypeDef;

// For bootloader
#define BOOTLOADER_MAGIC_ADDR ((uint32_t*) ((uint32_t) 0x20001000)) //4k into SRAM (out of 6k)
#define BOOTLOADER_MAGIC_TOKEN 0xDEADBEEF // :D//Value taken from CD00167594.pdf page 35, system memory start.
#define BOOTLOADER_START_ADDR 0x1fffc400//for ST32F042

USB_CMD_RetypeDef USB_CMD_Parse(uint8_t* buf,uint32_t* len);
void USB_CMD_RebootToBootloader();
void USB_CMD_BootSwitcher();
#endif
