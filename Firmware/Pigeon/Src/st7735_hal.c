#include "st7735_hal.h"

SPI_HandleTypeDef * hspi;
// static uint8_t ST7735_buffer[ST7735_WIDTH*2];
uint8_t current_x=0;
uint8_t current_y=0;

uint8_t isReady=0;

ST7735_ProfileTypDef *cur_profile;

ST7735_ProfileTypDef profiles[] = {
    {
        /* 旋转方向 左 */
        .XStart = 1,
        .YStart = 26,
        .Width  = 160,
        .Height = 80,
        .Rotate = (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR),
    },
    {
        /* 旋转方向 右 */
        .XStart = 1,
        .YStart = 26,
        .Width  = 160,
        .Height = 80,
        .Rotate = (ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR),
    },
    {
        /* 旋转方向 上 */
        .XStart = 26,
        .YStart = 1,
        .Width  = 80,
        .Height = 160,
        .Rotate = (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR),
    },
    {
        /* 旋转方向 下 */
        .XStart = 26,
        .YStart = 1,
        .Width  = 80,
        .Height = 160,
        .Rotate = (ST7735_MADCTL_BGR),
    },
};
ST7735_CMD_BUFTypeDef init_cmd[] = {
  // SWRESET Software reset 
  { 0, ST7735_SWRESET, 150, 0, {0}},
  // SLPOUT Leave sleep mode
  { 0, ST7735_SLPOUT,  150, 0, {0}},
  // FRMCTR1, FRMCTR2 Frame Rate configuration -- Normal mode, idle
  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D) 
  { 0, ST7735_FRMCTR1, 0, 3, { 0x01, 0x2C, 0x2D }},
  { 0, ST7735_FRMCTR2, 0, 3, { 0x01, 0x2C, 0x2D }},
  // FRMCTR3 Frame Rate configureation -- partial mode
  { 0, ST7735_FRMCTR3, 0, 6, { 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D }},
  // INVCTR Display inversion (no inversion)
  { 0, ST7735_INVCTR,  0, 1, { 0x07 }},
  // PWCTR1 Power control -4.6V, Auto mode
  { 0, ST7735_PWCTR1,  0, 3, { 0xA2, 0x02, 0x84 }},
  // PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
  { 0, ST7735_PWCTR2,  0, 1, { 0xC5 }},
  // PWCTR3 Power control, opamp current smal, boost frequency
  { 0, ST7735_PWCTR3,  0, 2, { 0x0A, 0x00 }},
  // PWCTR4 Power control, BLK/2, opamp current small and medium low
  { 0, ST7735_PWCTR4,  0, 2, { 0x8A, 0x2A }},
  // PWRCTR5, VMCTR1 Power control
  { 0, ST7735_PWCTR5,  0, 2, { 0x8A, 0xEE }},
  { 0, ST7735_VMCTR1,  0, 1, { 0x0E }},
  // INVOFF Don't invert display
  { 0, ST7735_INVOFF,  0, 0, {0}},
  // Memory access directions. row address/col address, bottom to top refesh (10.1.27)
  { 0, ST7735_MADCTL,  0, 1, {0}},
  // Color mode 16 bit (10.1.30
  { 0, ST7735_COLMOD,  0, 1, { 0x05 }},
  // Column address set 0..127 
  { 0, ST7735_CASET,   0, 4, { 0x00, 0x00, 0x00, 0x4F }},
  // Row address set 0..159
  { 0, ST7735_RASET,   0, 4, { 0x00, 0x00, 0x00, 0x9F }},
  { 0, ST7735_INVON,   0, 0, {0}},
  // GMCTRP1 Gamma correction
  { 0, ST7735_GMCTRP1, 0, 16, { 0x02, 0x1C, 0x07, 0x12,
                             0x37, 0x32, 0x29, 0x2D,
                             0x29, 0x25, 0x2B, 0x39,
                             0x00, 0x01, 0x03, 0x10 }},
  // GMCTRP2 Gamma Polarity corrction
  { 0, ST7735_GMCTRN1, 0, 16, { 0x03, 0x1d, 0x07, 0x06,
                             0x2E, 0x2C, 0x29, 0x2D,
                             0x2E, 0x2E, 0x37, 0x3F,
                             0x00, 0x00, 0x02, 0x10 }},
  // DISPON Display on
  { 0, ST7735_NORON, 100, 0, {0}},
  // NORON Normal on
  { 0, ST7735_DISPON,  10, 0, {0}},
  // End
  { 0, 0, 0, 0, {0}}
};

uint8_t ST7735_IsReady(){
    return isReady;
}

void ST7735_Select() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
}

void ST7735_Unselect() {
    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}

void ST7735_Reset() {
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);
}

void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &cmd, 1, HAL_MAX_DELAY);
}

void ST7735_WriteData(uint8_t* buff, uint32_t buff_size) {
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(hspi, buff, buff_size, HAL_MAX_DELAY);
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // column address set
    ST7735_WriteCommand(ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + cur_profile->XStart, 0x00, x1 + cur_profile->XStart };
    ST7735_WriteData(data, sizeof(data));

    // row address set
    ST7735_WriteCommand(ST7735_RASET);
    data[1] = y0 + cur_profile->YStart;
    data[3] = y1 + cur_profile->YStart;
    ST7735_WriteData(data, sizeof(data));

    // write to RAM
    ST7735_WriteCommand(ST7735_RAMWR);
}

void ST7735_SetProfile(uint8_t idx){
    cur_profile = &profiles[idx];
}

void ST7735_Init(SPI_HandleTypeDef * spi) {
    hspi=spi;
    ST7735_CMD_BUFTypeDef *cmd;

    // 复位ST7735
    ST7735_Unselect();
    ST7735_Reset();
    ST7735_Select();

    // 发送初始化指令
    for (cmd = init_cmd; cmd->command; cmd++)
    {
      ST7735_WriteCommand(cmd->command);
      if (cmd->len){
        ST7735_WriteData((cmd->command == ST7735_MADCTL?&cur_profile->Rotate:cmd->data),cmd->len);
      }
      if (cmd->delay)
        HAL_Delay(cmd->delay);
    }

    ST7735_Unselect();
    isReady=1;
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= cur_profile->Width) || (y >= cur_profile->Height))
        return;

    ST7735_Select();

    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    ST7735_Unselect();
}

void ST7735_DrawBuffer(uint8_t* buf,uint16_t len) {
    ST7735_Select();
    ST7735_WriteData(buf, len);
    ST7735_Unselect();
}

void ST7735_SetDrawArea(uint16_t startX, uint16_t startY,uint16_t endX, uint16_t endY) {
    ST7735_Select();
    ST7735_SetAddressWindow(startX, startY, endX-1, endY-1);
    ST7735_Unselect();
}
void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;

    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
}

uint8_t ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= cur_profile->Width) {
            x = 0;
            y += font.height;
            if(y + font.height >= cur_profile->Height) {
                break;
            }
            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width - 1;
        str++;
    }

    ST7735_Unselect();
    return x;
}
void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    if((x >= cur_profile->Width) || (y >= cur_profile->Height)) return;
    if((x + w - 1) >= cur_profile->Width) w = cur_profile->Width - x;
    if((y + h - 1) >= cur_profile->Height) h = cur_profile->Height - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { color >> 8, color & 0xFF };
    HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
    for(y = h; y > 0; y--) {
        for(x = w; x > 0; x--) {
            ST7735_WriteData(data, sizeof(data));
        }
    }

    ST7735_Unselect();
}

// void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
//   uint8_t ST7735_buffer[ST7735_WIDTH];
//     // clipping
//     if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
//     if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
//     if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

//     ST7735_Select();

//     for (int i = 0; i < sizeof(ST7735_buffer);i++)
//     {
//       ST7735_buffer[i]=color >> 8;
//       i++;
//       ST7735_buffer[i]=color & 0xFF;
//     }

//     HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
//     for(y = 0; h>y; y+=2) {
//       ST7735_SetAddressWindow(0, y, w-1, y);
//       ST7735_WriteData(ST7735_buffer, sizeof(ST7735_buffer));
//       ST7735_WriteData(ST7735_buffer, sizeof(ST7735_buffer));
//     }
//     for(y = 1; h>y; y+=2) {
//       ST7735_SetAddressWindow(0, y, w-1, y);
//       ST7735_WriteData(ST7735_buffer, sizeof(ST7735_buffer));
//       ST7735_WriteData(ST7735_buffer, sizeof(ST7735_buffer));
//     }

//     ST7735_Unselect();
// }

void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, cur_profile->Width, cur_profile->Height, color);
    current_y=0;
    current_x=0;
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= cur_profile->Width) || (y >= cur_profile->Height)) return;
    if((x + w - 1) >= cur_profile->Width) return;
    if((y + h - 1) >= cur_profile->Height) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}

void ST7735_print(const char* str,FontDef font, uint16_t color, uint16_t bgcolor){
  ST7735_Select();

  while(*str) {
    if(current_x>= cur_profile->Width) {
      current_x = 0;
      current_y += font.height;
      if(current_y + font.height >= cur_profile->Height) {
        current_y=0;
      }

      if(*str == ' ') {
        // skip spaces in the beginning of the new line
        str++;
        continue;
      }
    }
    if(*str == '\n') {
      // 换行符处理
      str++;
      current_x = 0;
      if(current_y + font.height >= cur_profile->Height)
        current_y=0;
      else
        current_y += font.height;
      continue;
    }
    if(*str == 0) {
      break;
    }

    ST7735_WriteChar(current_x, current_y, *str, font, color, bgcolor);
    str++;
    current_x += font.width - 1;
  }

  ST7735_Unselect();
}
void ST7735_println(const char* str,FontDef font, uint16_t color, uint16_t bgcolor){
    ST7735_Select();

  while(*str) {
    if(current_x + font.width >= cur_profile->Width) {
      current_x = 0;
      current_y += font.height;
      if(current_y + font.height >= cur_profile->Height) {
        current_y=0;
      }

      if(*str == ' ') {
        // skip spaces in the beginning of the new line
        str++;
        continue;
      }
    }
    if(*str == '\n') {
      // 换行符处理
      str++;
      current_x = 0;
      if(current_y + font.height >= cur_profile->Height)
        current_y=0;
      else
        current_y += font.height;
      continue;
    }

    ST7735_WriteChar(current_x, current_y, *str, font, color, bgcolor);
    current_x += font.width - 1;
    str++;
  }
  //换行
  current_x = 0;
  if(current_y + font.height >= cur_profile->Height)
    current_y=0;
  else
    current_y += font.height;
  ST7735_Unselect();
}

void ST7735_DrawRLE(uint16_t *colorTab, uint16_t *data, uint16_t len){
    ST7735_SetDrawArea(0,0,cur_profile->Width,cur_profile->Height);
    ST7735_Select();
    // Decode RLE Data
    for(uint16_t i = 0; i < len; i++){
        uint8_t colorIdx = (data[i] >> 14) & 0xff;
        uint16_t pixelLen = data[i] & 0x3fff;

        for (uint16_t j = 0; j < pixelLen; j++){
            uint8_t pixel[] = { colorTab[colorIdx] >> 8, colorTab[colorIdx] & 0xFF };
            ST7735_WriteData(pixel, sizeof(pixel)/sizeof(uint8_t));
        }
    }
}
