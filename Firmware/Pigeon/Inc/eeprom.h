#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "st7735_hal.h"

typedef struct EEPROM{
    uint8_t  ProfileIdx;
    uint8_t  Brightness;
    uint16_t ColorTab[4];
    uint16_t RLELen;
    uint16_t RLEData[];
} EEPROM_TypeDef;

EEPROM_TypeDef * EEPROM_GetData();
#endif //__EEPROM_H__
