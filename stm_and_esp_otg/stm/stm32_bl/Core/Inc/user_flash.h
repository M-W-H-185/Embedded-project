#ifndef __USER_FLASH_H
#define __USER_FLASH_H
#include "main.h"

#define STM32_FLASH_SIZE 256            //所选STM32的FLASH容量大小(单位为K)
 
/* FLASH起始地址 */
#define STM32_FLASH_BASE 0x08000000     //STM32 FLASH的起始地址
 
/* 小容量和中容量产品每页1K字节（1024 Byte）、大容量每页2K字节（2048 Byte）*/
#if STM32_FLASH_SIZE < 256
#define STM_SECTOR_SIZE 1024
#else 
#define STM_SECTOR_SIZE 2048
#endif

int8_t STMFLASH_WriteMultipleBytes(uint32_t WriteAddr,uint16_t  *pBuffer,uint16_t  NumToWrite);

#endif
