#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__
#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"
typedef     unsigned char    uint8_t;	//  8 bits 

typedef     unsigned int     uint16_t;	// 16 bits 

typedef     unsigned long    uint32_t;	// 32 bits 

unsigned char Read_Byte(unsigned char reg); /* 读取寄存器数据函数 */


void QMA7981Init(void);

#endif

