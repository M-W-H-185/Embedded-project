#ifndef __HAL_I2C_H__
#define __HAL_I2C_H__
#include "stc8g.h"
#include "intrins.h"
#include "stdio.h"
#include "string.h"
typedef     unsigned char    uint8_t;	//  8 bits 

typedef     unsigned int     uint16_t;	// 16 bits 

typedef     unsigned long    uint32_t;	// 32 bits 


void hal_I2cInit(void);
// ������ʼ�ź�
uint8_t hal_I2cStart(void);
// ����ֹͣ�ź�
uint8_t hal_I2cStop(void);
// ����byte
uint8_t hal_I2cSendByte(uint8_t _data);
// ��ȡһ���ֽ�
uint8_t hal_I2cReadByteData(void);
// ����һ��nack
uint8_t hal_I2cSendNACK(void);
	void hal_I2cRendACK();
	

#endif

