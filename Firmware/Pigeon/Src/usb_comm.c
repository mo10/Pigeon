#include "usb_comm.h"
#include "settings.h"

uint16_t dataSize = 0;
uint8_t *data;

void Pigeon_Comm_Parse(uint8_t *buf,uint32_t len)
{
  Pigeon_Comm_DataTypeDef *recvData = (Pigeon_Comm_DataTypeDef *)buf;

  if(recvData->Head != PIGEON_DATA_HEAD)
    return;

  switch(recvData->Cmd)
  {
    //读取设置
    case READ_SETTINGS:
    {
      Pigeon_Comm_DataTypeDef *preData;
      Pigeon_SettingsTypeDef *currentSettings = Pigeon_GetSettings();
      // 计算所需数据长度
      uint16_t dataLen = sizeof(Pigeon_Comm_DataTypeDef) + sizeof(Pigeon_SettingsTypeDef) + currentSettings->RLELen;
      // 开辟内存 填充数据
      preData = (Pigeon_Comm_DataTypeDef *)malloc(dataLen);
      preData->Head = PIGEON_DATA_HEAD;
      preData->Cmd = READ_SETTINGS;
      preData->Len = sizeof(Pigeon_SettingsTypeDef) + currentSettings->RLELen;
      memcpy(preData->Data, currentSettings, preData->Len);
      // 等Poll函数调用发送
      data = (uint8_t *)preData;
      dataSize = dataLen;
      
      break;
    }
  }
}

void Pigeon_Comm_Poll()
{
  if(dataSize > 0)
  {
    uint16_t offset = 0;
    while((dataSize - offset) > 0){
      uint16_t packSize = (dataSize - offset) >= 64 ? 64 : (dataSize - offset);
      if(packSize > 0){
        ST7735_FillScreen(ST7735_GREEN);
        USBD_SendReport_FS(WINUSB_EPIN_ADDR, data + offset, packSize);
        offset += packSize;
      }
      
    }
    // 清理
    dataSize = 0;
    free(data);
  }
}