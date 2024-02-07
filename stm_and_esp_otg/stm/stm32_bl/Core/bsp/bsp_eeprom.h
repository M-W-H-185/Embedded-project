#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H

#include "main.h"

// flash ÿһҳ�Ĵ�С
#ifndef FLASH_PAGE_SIZE
    #define FLASH_PAGE_SIZE 2048
#endif

// ����flashģ��eeprom �����һҳ��Ϊģ��eeprom
#define EEPROM_FLASH_START_ADDRESS  (0x800FFFF - FLASH_PAGE_SIZE -1)
#define EEPROM_FLASH_END_ADDRESS    (EEPROM_FLASH_START_ADDRESS+FLASH_PAGE_SIZE)

void EEPROM_ReadWords(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  ;

void EEPROM_WriteByte(uint16_t StartAddr, uint16_t data)  ;

void EEPROM_WriteBytes(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  ;

#endif

