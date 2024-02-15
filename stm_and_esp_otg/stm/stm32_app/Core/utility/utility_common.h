#ifndef __UTILITY_COMMON_H_
#define __UTILITY_COMMON_H_
#include <stdlib.h>
#include "stdint.h"
#include "string.h"



#define COMBINE_BYTES_TO_UINT16(high_byte, low_byte) (((uint16_t)(high_byte) << 8) | (low_byte))
#define UINT16_HIGH_BYTE(value) ((uint8_t)(((value) >> 8) & 0xFF))
#define UINT16_LOW_BYTE(value) ((uint8_t)((value) & 0xFF))


// 计算 CRCCRC-16/MODBUS
// crc modbus 校验和遵循 低位在前 高位在后
// https://blog.csdn.net/m0_37697335/article/details/113267780
uint16_t calculateCRC(uint8_t *data, int length);

int user_ceil(float num) ;
uint16_t combineUint8ArrayToUint16(uint8_t *inputArray);

void splitUint16ToUint8Array(uint16_t input, uint8_t *outputArray);
uint32_t combineUint8ArrayToUint32( uint8_t *inputArray);

void splitUint32ToArray(uint32_t input, uint8_t *outputArray);

// uint8数组转uint16
void convertUint8ToUint16(uint8_t *inputArray, uint16_t *outputArray, size_t arraySize, size_t outputArraySize);

// 将uint16转换为uint8数组
void convertUint16ToUint8(uint16_t *uint16Array, uint8_t *uint8Array, size_t uint16ArraySize,size_t uint8ArraySize);
#endif
