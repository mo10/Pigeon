#ifndef __EEPROM_H__
#define __EEPROM_H__

#include "st7735_hal.h"
#include "keycode.h"

typedef struct EEPROM{
    uint8_t  ProfileIdx;
    uint8_t  KeyDef;
    uint8_t  Brightness;
    uint16_t ColorTab[4];
    uint16_t RLELen;
    uint16_t RLEData[];
} EEPROM_TypeDef;

typedef enum Key
{
    EEPROM_KEY_NONE,
    EEPROM_KEY_KBD,
    EEPROM_KEY_EVENT,
    EEPROM_KEY_BOTH,
} EEPROM_KeyTypeDef;

EEPROM_TypeDef * EEPROM_GetData();

#endif //__EEPROM_H__
