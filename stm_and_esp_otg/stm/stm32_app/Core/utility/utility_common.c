#include "utility_common.h"

#include <stdlib.h>
#include "stdint.h"
#include "string.h"


// ���� CRCCRC-16/MODBUS
// crc modbus У�����ѭ ��λ��ǰ ��λ�ں�
// https://blog.csdn.net/m0_37697335/article/details/113267780
uint16_t calculateCRC(uint8_t *data, int length) {
    unsigned int i;
    unsigned short crc = 0xFFFF;  //crc16λ�Ĵ�����ʼֵ
    // printf("crc16_modbusУ��->length:%d data:",length);
    while(length--)
    {
        // printf("%02x ",*data);
        crc ^= *data++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001; //����ʽ POLY��0x8005)�ĸߵ�λ����ֵ������������ģ�͵�һЩ����������
            }
            else
            {
                crc = (crc >> 1);
            }
        }
    }
    // printf("\r\n");
    return crc;
}

int user_ceil(float num) {
    int inum = (int)num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}
uint16_t combineUint8ArrayToUint16(uint8_t *inputArray) {
    // �ϲ�����Ԫ��Ϊ uint32_t
    uint16_t result = 0;
    result |= ((uint16_t)inputArray[0] << 8);
    result |= inputArray[1];
    return result;
}

void splitUint16ToUint8Array(uint16_t input, uint8_t *outputArray) {
    // ��ֲ��洢������
    outputArray[0] = (uint8_t)((input >> 8) & 0xFF);
    outputArray[1] = (uint8_t)(input & 0xFF);
}

uint32_t combineUint8ArrayToUint32( uint8_t *inputArray) {
    // �ϲ�����Ԫ��Ϊ uint32_t
    uint32_t result = 0;
    result |= ((uint32_t)inputArray[0] << 24);
    result |= ((uint32_t)inputArray[1] << 16);
    result |= ((uint32_t)inputArray[2] << 8);
    result |= inputArray[3];
    return result;
} 

void splitUint32ToArray(uint32_t input, uint8_t *outputArray) {
    // ��ֲ��洢������
    outputArray[0] = (uint8_t)((input >> 24) & 0xFF);
    outputArray[1] = (uint8_t)((input >> 16) & 0xFF);
    outputArray[2] = (uint8_t)((input >> 8) & 0xFF);
    outputArray[3] = (uint8_t)(input & 0xFF);
}
// ������util

// uint8����תuint16
void convertUint8ToUint16(uint8_t *inputArray, uint16_t *outputArray, size_t arraySize, size_t outputArraySize) {
	// ��uint8����ת��Ϊuint16����
	for (size_t i = 0; i < arraySize; i += 2) {
		outputArray[i / 2] = COMBINE_BYTES_TO_UINT16(inputArray[i + 1] ,inputArray[i]);
	}
	// ��������С����������0xff
	if (arraySize % 2 != 0) {
		outputArray[outputArraySize-1] = COMBINE_BYTES_TO_UINT16(0xff   ,inputArray[arraySize-1]);
	}
}

// ��uint16ת��Ϊuint8����
void convertUint16ToUint8(uint16_t *uint16Array, uint8_t *uint8Array, size_t uint16ArraySize,size_t uint8ArraySize) {
    

    size_t i;
    volatile uint16_t temp_debug =  0 ;
    // ��uint16_t����ת��Ϊuint8_t����
    for (i = 0; i < uint16ArraySize; ++i) {
        temp_debug = uint16Array[i];
        uint8Array[2 * i] = UINT16_LOW_BYTE(temp_debug);       // ���ֽ�
        uint8Array[2 * i + 1] = UINT16_HIGH_BYTE(temp_debug);  // ���ֽ�
    }

    // ����uint8ArraySize�����СΪ���������, �Ǿʹ�����ʱ�������һλ����Ч�� 
    if (uint8ArraySize % 2 != 0) {
        uint8Array[2 * i - 1] = 0x00;  // ���ֽ�
    }
}
