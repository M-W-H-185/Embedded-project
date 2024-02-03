#ifndef __USER_FLASH_H
#define __USER_FLASH_H
#include "main.h"

#define STM32_FLASH_SIZE 256            //��ѡSTM32��FLASH������С(��λΪK)
 
/* FLASH��ʼ��ַ */
#define STM32_FLASH_BASE 0x08000000     //STM32 FLASH����ʼ��ַ
 
/* С��������������Ʒÿҳ1K�ֽڣ�1024 Byte����������ÿҳ2K�ֽڣ�2048 Byte��*/
#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024
#else 
#define STM_SECTOR_SIZE 2048
#endif

int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite);

#endif
