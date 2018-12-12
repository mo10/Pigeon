#include "usb_cmd.h"
/*
  USB 控制命令协议
  PC->STM32
  uint8_t head = 0x22;        // 开始
  uint8_t cmd = 0;            // 命令:绘制纯色矩形 图片刷屏 打印文字 反色 设置参数(屏亮度,屏幕初始颜色) 终止数据传输
  uint16_t length;            // 数据长度:接下来的包数据长度
  uint8_t data1;              // 附加数据(参数)
  uint8_t data2;              // 附加数据(参数)
  uint8_t data3;              // 附加数据(参数)
  uint8_t data4;              // 附加数据(参数)
  uint8_t end = 0x33;         // 结束
  总长度9bytes

  STM32->PC
  uint8 status                // OK=0,请求下一个数据包=1

  传输模型:
  |--Command Package(9bytes)--| |--Data Package1(Max length 64bytes)--| |--Data Package2--| |--Data Package3--| etc....
*/
uint16_t recv_data_len=0;
uint16_t need_data_len=0;
bool isRecv=false;
//解析USB数据
USB_CMD_RetypeDef USB_CMD_Parse(uint8_t* buf,uint32_t* len){
  USB_CMD_BUFTypeDef* cmd_buf = (USB_CMD_BUFTypeDef*)buf;
  
  if(*len == sizeof(USB_CMD_BUFTypeDef) /* 长度检查 */
    && cmd_buf->head == CMD_BUF_HEAD  /* 标志头检查 */
    && cmd_buf->end == CMD_BUF_END){  /* 标志尾检查 */
    switch(cmd_buf->cmd){
      case USB_CMD_DRAWIMAGE:
        // 绘制图片
        ST7735_SetDrawArea(cmd_buf->data[0], cmd_buf->data[1],
                            cmd_buf->data[2], cmd_buf->data[3]);
        recv_data_len=0;
        need_data_len=cmd_buf->length;
        isRecv=true;
        return USB_CMD_OK;
      case USB_CMD_STOP:
        // 停止传输
        recv_data_len=0;
        need_data_len=0;
        isRecv=false;
        return USB_CMD_OK;
      case USB_CMD_TOBOOTLOADER:
        USB_CMD_RebootToBootloader();
        return USB_CMD_OK;
    }
  }else if(*len > 0 && isRecv){
    // 包长度大于0 并且已设置传输标志
    uint16_t pre_len = *len + recv_data_len; // 预计算已接收数据量
    if (pre_len < need_data_len || pre_len == need_data_len){
      // 预接收长度小于等于需要数据量
      ST7735_DrawBuffer(buf,*len); // 在屏幕上绘图
      recv_data_len = pre_len;
      if (recv_data_len == need_data_len)
        isRecv=false;
    }else if(pre_len > need_data_len){
      // 超出设置长度范围,停止绘制
      recv_data_len=0;
      need_data_len=0;
      isRecv=false;
      return USB_CMD_FAIL;
    }
  }
  return USB_CMD_OK;
}

//This code from:
//https://community.st.com/s/question/0D50X00009Xkg1VSAR/jump-to-usb-dfu-bootloader-in-startup-code-on-stm32f042
void USB_CMD_RebootToBootloader(){
//call this at any time to initiate a reboot into bootloader
*BOOTLOADER_MAGIC_ADDR = BOOTLOADER_MAGIC_TOKEN;
NVIC_SystemReset();
}
//This is the first thing the micro runs after startup_stm32f0xx.s
//Only the SRAM is initialized at this point
void USB_CMD_BootSwitcher(){
  uint32_t jumpaddr,tmp;
  tmp=*BOOTLOADER_MAGIC_ADDR;
  if (tmp == BOOTLOADER_MAGIC_TOKEN){
    // 要求转跳Bootloader
    *BOOTLOADER_MAGIC_ADDR=0; //重置回0
    // 初始化Boot0引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOB, BOOT0_Pin, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = BOOT0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BOOT0_GPIO_Port, BOOT0_Pin, GPIO_PIN_SET); // 拉高Boot0
    
    void (*bootloader)(void) = 0; //Zero the function pointer.
    jumpaddr = *(__IO uint32_t*)(BOOTLOADER_START_ADDR + 4);
    bootloader = (void (*)(void)) jumpaddr; //Set the function pointerto bootaddr +4
    __set_MSP(*(__IO uint32_t*) BOOTLOADER_START_ADDR); //load the stackpointer - bye bye program
    bootloader(); //Go to DFU mode
    //this should never be hit, trap for debugging
    while(1){}
  }
}