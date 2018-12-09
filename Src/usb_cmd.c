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
