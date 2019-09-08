#ifndef __USB_COMM_H__
#define __USB_COMM_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "usbd_custom_hid_if.h"

#define PIGEON_DATA_HEAD 0x66

typedef enum
{
  READ_SETTINGS = 0,
  SAVE_SETTINGS,
} Pigeon_Comm_CmdTypeDef;

typedef struct Pigeon_Comm_Data
{
  uint8_t Head;
  uint8_t Cmd;
  uint16_t Len;
  uint8_t Data[0];
} Pigeon_Comm_DataTypeDef;

void Pigeon_Comm_Parse(uint8_t *buf,uint32_t len);
void Pigeon_Comm_Poll();
#endif // __USB_COMM_H__
