#include "bootloader.h"

void BOOTLOADER_Jump(){
    //https://community.st.com/s/question/0D50X00009Xkg1VSAR/jump-to-usb-dfu-bootloader-in-startup-code-on-stm32f042
    //call this at any time to initiate a reboot into bootloader
    *BOOTLOADER_MAGIC_ADDR = BOOTLOADER_MAGIC_TOKEN;
    NVIC_SystemReset();
}
void BOOTLOADER_Init(){
  uint32_t jumpaddr;
  if (*BOOTLOADER_MAGIC_ADDR == BOOTLOADER_MAGIC_TOKEN){
    // 要求转跳Bootloader
    *BOOTLOADER_MAGIC_ADDR=0;
    // 初始化Boot0引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET); // 拉高Boot0
    
    void (*bootloader)(void) = 0; //Zero the function pointer.
    jumpaddr = *(__IO uint32_t*)(BOOTLOADER_START_ADDR + 4);
    bootloader = (void (*)(void)) jumpaddr; //Set the function pointerto bootaddr +4
    __set_MSP(*(__IO uint32_t*) BOOTLOADER_START_ADDR); //load the stackpointer - bye bye program
    bootloader(); //Go to DFU mode
    //this should never be hit, trap for debugging
    while(1){}
  }
}
