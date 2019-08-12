#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "main.h"

#define BOOTLOADER_MAGIC_ADDR ((uint32_t*) ((uint32_t) 0x20001000)) //4k into SRAM (out of 6k)
#define BOOTLOADER_MAGIC_TOKEN 0xDEADBEEF // :D//Value taken from CD00167594.pdf page 35, system memory start.
#define BOOTLOADER_START_ADDR 0x1fffc400  //for ST32F042

void BOOTLOADER_Init();
void BOOTLOADER_Jump();

#endif //__BOOTLOADER_H__
