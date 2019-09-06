#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "st7735_hal.h"
#include "keycode.h"

typedef struct Pigeon_Settings{
    uint8_t  ProfileIdx;
    uint8_t  KeyDef;
    uint8_t  Brightness;
    uint16_t ColorTab[4];
    uint16_t RLELen;
    uint16_t RLEData[];
} Pigeon_SettingsTypeDef;

typedef enum Key
{
    EEPROM_KEY_NONE,
    EEPROM_KEY_KBD,
    EEPROM_KEY_EVENT,
    EEPROM_KEY_BOTH,
} EEPROM_KeyTypeDef;

Pigeon_SettingsTypeDef *Pigeon_GetSettings();

#endif //__SETTINGS_H__
