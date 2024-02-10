#include "app_otaConfig.h"
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "main.h"
#include "bsp_eeprom.h"
#include "SEGGER_RTT.h"
int user_ceil(float num) {
    int inum = (int)num;
    if (num == (float)inum) {
        return inum;
    }
    return inum + 1;
}

uint32_t combineUint8ArrayToUint32( uint8_t *inputArray) {
    // 合并数组元素为 uint32_t
    uint32_t result = 0;
    result |= ((uint32_t)inputArray[0] << 24);
    result |= ((uint32_t)inputArray[1] << 16);
    result |= ((uint32_t)inputArray[2] << 8);
    result |= inputArray[3];
    return result;
} 

void splitUint32ToArray(uint32_t input, uint8_t *outputArray) {
    // 拆分并存储到数组
    outputArray[0] = (uint8_t)((input >> 24) & 0xFF);
    outputArray[1] = (uint8_t)((input >> 16) & 0xFF);
    outputArray[2] = (uint8_t)((input >> 8) & 0xFF);
    outputArray[3] = (uint8_t)(input & 0xFF);
}
// 上面是util

// app 版本 begin
// 设置app版本
void app_otaSetVersion(char *version_str, uint8_t version_length)
{
    // 设置app字符串长度
    EEPROM_WriteByte(APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_START, version_length);
    // 设置app版本
    uint16_t temp_size = user_ceil((float)((float)version_length/(float)2.000));
    uint16_t *temp = malloc(temp_size);
    memset(temp, 0xff, temp_size);
    
    convertUint8ToUint16((uint8_t*)version_str,temp,version_length,temp_size);
    
    EEPROM_WriteBytes(APP_VERSION_STR_EEPROM_ADDRESS_START,temp,version_length);

}
// 获取app版本
uint8_t app_otaGetVersion(char *version_str)
{
    uint8_t version_length = (uint8_t) EEPROM_ReadWord(APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_START);
    
    uint16_t read_eeprom_array_size = user_ceil((float)((float)version_length/(float)2.000));
    uint16_t *read_eeprom_array = malloc(read_eeprom_array_size);
    memset(read_eeprom_array, 0xff, read_eeprom_array_size);
    EEPROM_ReadWords(APP_VERSION_STR_EEPROM_ADDRESS_START, read_eeprom_array, read_eeprom_array_size);
    
    
    SEGGER_RTT_printf(0, "app_otaSetVersion(function):read_eeprom_array(uint16_t):"); 
    /**for(uint8_t i = 0;i<read_eeprom_array_size;i++)
    {
        SEGGER_RTT_printf(0, "%04x ",read_eeprom_array[i]); 
    }
    SEGGER_RTT_printf(0, "--->"); **/

    
    convertUint16ToUint8(read_eeprom_array, (uint8_t*)version_str, read_eeprom_array_size, version_length);
    /**SEGGER_RTT_printf(0, "Uint16 To Uint8 of data:"); 
    for(uint8_t i = 0;i<version_length;i++)
    {
        SEGGER_RTT_printf(0, "%02x ",version_str[i]); 
    }
    SEGGER_RTT_printf(0, "\r\n"); **/

    return version_length;

}
// app 版本 end


// app固件大小 以及 crc校验和 begin

// 保存App固件大小
void app_otaSetFirmwareSize(uint32_t firmwareSize)
{

    uint8_t uint32_byte_array[4] = {0x00};
    memset(uint32_byte_array, 0x00, 4);
    splitUint32ToArray(firmwareSize, uint32_byte_array); // 将uint32 拆分并存储到uint8数组
    
    // 将uint8转换为uint16
    uint16_t wirteEEPROM_data[2] = {0,0};
    memset(wirteEEPROM_data, 0x00, 2);
    convertUint8ToUint16(uint32_byte_array, wirteEEPROM_data, 4, 2);

    EEPROM_WriteBytes(APP_FIRMWARE_SIZE_EEPROM_ADDRESS_START, wirteEEPROM_data, 2);
    
}
// 获取App固件大小
uint32_t app_otaGetFirmwareSize(void)
{
    uint16_t read_eeprom_data[2] = {0,0};
    memset(read_eeprom_data, 0x00, 4);
    EEPROM_ReadWords(APP_FIRMWARE_SIZE_EEPROM_ADDRESS_START, read_eeprom_data, 2);
    
    uint8_t u16Tou8_data[4] = {0,0,0,0};
    convertUint16ToUint8(read_eeprom_data, u16Tou8_data, 2, 4);
    
    return combineUint8ArrayToUint32(u16Tou8_data);
}

// 保存App固件crc校验和
void app_otaSetFirmwareCrcChecksum(uint32_t firmwareCrcChecksum)
{

    uint8_t uint32_byte_array[4] = {0x00};
    memset(uint32_byte_array, 0x00, 4);
    splitUint32ToArray(firmwareCrcChecksum, uint32_byte_array); // 将uint32 拆分并存储到uint8数组
    
    // 将uint8转换为uint16
    uint16_t wirteEEPROM_data[2] = {0,0};
    memset(wirteEEPROM_data, 0x00, 2);
    convertUint8ToUint16(uint32_byte_array, wirteEEPROM_data, 4, 2);

    EEPROM_WriteBytes(APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START, wirteEEPROM_data, 2);
    
}
// 获取App固件crc校验和
uint32_t app_otaGetFirmwareCrcChecksum(void)
{
    uint16_t read_eeprom_data[2] = {0,0};
    memset(read_eeprom_data, 0x00, 4);
    EEPROM_ReadWords(APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START, read_eeprom_data, 2);
    
    uint8_t u16Tou8_data[4] = {0,0,0,0};
    convertUint16ToUint8(read_eeprom_data, u16Tou8_data, 2, 4);
    
    return combineUint8ArrayToUint32(u16Tou8_data);
}



// app固件大小 以及 crc校验和 end

/** App固件 版本字符串、大小、crc校验和测试代码


//// 获取App固件版本
//char tempStr[] = "0.0.1";
//app_otaSetVersion(tempStr,strlen(tempStr));
//SEGGER_RTT_printf(0, "ota set app of version :%s %d\r\n", tempStr, strlen(tempStr)); 


//// 获取App固件版本
//char tempReadStr[10] = { 0 };
//memset(tempReadStr,0x00,10);
//app_otaGetVersion(tempReadStr);
//SEGGER_RTT_printf(0, "ota get app of version: %s--->%d \r\n", tempReadStr,strlen(tempReadStr)); 


//// 保存App固件大小 0xFFFFFFA1
//app_otaSetFirmwareSize(0xFFFFFFA1);
//// 获取App固件大小
//SEGGER_RTT_printf(0, "app_otaGetFirmwareSize:%08x \r\n",app_otaGetFirmwareSize());  
//HAL_Delay(1);


//// 保存App固件crc校验和
//app_otaSetFirmwareCrcChecksum(0x88FFFF66);
//// 获取App固件crc校验和
//SEGGER_RTT_printf(0, "app_otaGetFirmwareCrcChecksum:%08x \r\n",app_otaGetFirmwareCrcChecksum());  
//HAL_Delay(1);

**/


