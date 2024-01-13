#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__
#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"

void i2c_init(void);

/* IIC写一个字节 
   reg:寄存器地址
   dat:数据 */
void Write_Byte(unsigned char reg, unsigned char dat);

/* IIC读一个字节  
   reg:寄存器地址 
   返回值:读到的数据 */
unsigned char Read_Byte(unsigned char reg);


#endif

