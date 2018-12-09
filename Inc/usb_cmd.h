#ifndef __USB_CMD_H__
#define __USB_CMD_H__

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
  USB_CMD_FILLRECT
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

#endif
