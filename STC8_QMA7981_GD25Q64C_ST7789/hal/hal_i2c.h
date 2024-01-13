#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__
#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"

void i2c_init(void);

/* IICдһ���ֽ� 
   reg:�Ĵ�����ַ
   dat:���� */
void Write_Byte(unsigned char reg, unsigned char dat);

/* IIC��һ���ֽ�  
   reg:�Ĵ�����ַ 
   ����ֵ:���������� */
unsigned char Read_Byte(unsigned char reg);


#endif

