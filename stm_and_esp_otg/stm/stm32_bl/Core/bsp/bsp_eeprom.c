#include "main.h"
#include <stdlib.h>
#include "string.h"
#include "bsp_eeprom.h"
#include "user_flash.h"


/*------------------------------------------------------------ 
 Func: EEPROM数据读出  需要两字节对齐
 Note: 
-------------------------------------------------------------*/  
void EEPROM_ReadWords(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  
{  
    if((StartAddr%2) != 0)
    {
        return;
    }
    
    if(StartAddr > EEPROM_FLASH_START_ADDRESS + StartAddr)
    {
        return;
    }
    STMFLASH_ReadMultipleBytes(EEPROM_FLASH_START_ADDRESS + StartAddr, buffer, buff_size);

} 





/*------------------------------------------------------------ 
 Func: EEPROM数据按字节写入 
 Note: 
-------------------------------------------------------------*/  
void EEPROM_WriteBytes(uint16_t StartAddr, uint16_t *buffer, uint16_t buff_size)  
{
    if(StartAddr > EEPROM_FLASH_START_ADDRESS + StartAddr)
    {
        return;
    }
    STMFLASH_WriteMultipleBytes( EEPROM_FLASH_START_ADDRESS + StartAddr , buffer, buff_size);

}

void EEPROM_WriteByte(uint16_t StartAddr, uint16_t data)  
{
    if(StartAddr > EEPROM_FLASH_START_ADDRESS + StartAddr)
    {
        return;
    }
    uint16_t buffer[1];  // 创建包含一个uint16_t元素的缓冲区

    buffer[0] = data;  // 将要写入EEPROM的数据放入缓冲区
    
    STMFLASH_WriteMultipleBytes( EEPROM_FLASH_START_ADDRESS + StartAddr , buffer, 1);

}

    