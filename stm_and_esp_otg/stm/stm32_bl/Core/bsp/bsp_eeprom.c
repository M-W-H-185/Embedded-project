#include "main.h"
#include <stdlib.h>
#include "string.h"
#include "bsp_eeprom.h"
#include "user_flash.h"


/*------------------------------------------------------------ 
 Func: EEPROM���ݶ���  ��Ҫ���ֽڶ���
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

int16_t EEPROM_ReadWord(uint16_t StartAddr)  
{  
    if((StartAddr%2) != 0)
    {
        return -1;
    }
    
    if(StartAddr > EEPROM_FLASH_START_ADDRESS + StartAddr)
    {
        return -2;
    }
    uint16_t temp[] = { 0x0000 }; 
    STMFLASH_ReadMultipleBytes(EEPROM_FLASH_START_ADDRESS + StartAddr, temp, 1);
    return temp[0];

} 




/*------------------------------------------------------------ 
 Func: EEPROM���ݰ��ֽ�д�� 
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
    uint16_t buffer[1];  // ��������һ��uint16_tԪ�صĻ�����

    buffer[0] = data;  // ��Ҫд��EEPROM�����ݷ��뻺����
    
    STMFLASH_WriteMultipleBytes( EEPROM_FLASH_START_ADDRESS + StartAddr , buffer, 1);

}

    