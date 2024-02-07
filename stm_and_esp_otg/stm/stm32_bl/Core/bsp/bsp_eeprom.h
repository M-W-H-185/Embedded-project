#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H

#include "main.h"

// flash 每一页的大小
#ifndef FLASH_PAGE_SIZE
    #define FLASH_PAGE_SIZE 2048
#endif

// 采用flash模拟eeprom 用最后一页作为模拟eeprom
#define EEPROM_FLASH_START_ADDRESS  (0x800FFFF - FLASH_PAGE_SIZE -1)
#define EEPROM_FLASH_END_ADDRESS    (EEPROM_FLASH_START_ADDRESS+FLASH_PAGE_SIZE)

void EEPROM_ReadWords(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  ;

void EEPROM_WriteByte(uint16_t StartAddr, uint16_t data)  ;

void EEPROM_WriteBytes(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  ;

#endif

