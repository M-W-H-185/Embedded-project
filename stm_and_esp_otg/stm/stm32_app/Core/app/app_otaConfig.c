/**
* App固件升级相关配置
*
**/
#include "app_otaConfig.h"
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "main.h"
#include "bsp_eeprom.h"
#include "SEGGER_RTT.h"
#include "utility_common.h"


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
    
    
    /**SEGGER_RTT_printf(0, "app_otaSetVersion(function):read_eeprom_array(uint16_t):"); 
    for(uint8_t i = 0;i<read_eeprom_array_size;i++)
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

// 保存App固件状态
void app_otaSetFirmwareState(uint16_t firmwareState)
{
    uint8_t uint16_byte_array[2] = {0x00};
    memset(uint16_byte_array, 0x00, 2);
    splitUint16ToUint8Array(firmwareState, uint16_byte_array); // 将uint16 拆分并存储到uint8数组
    
    // 将uint8转换为uint16
    uint16_t wirteEEPROM_data[1] = {0};
    memset(wirteEEPROM_data, 0x00, 1);
    convertUint8ToUint16(uint16_byte_array, wirteEEPROM_data, 2, 1);

    EEPROM_WriteBytes(APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START, wirteEEPROM_data,1);
}


// 获取App固件状态
uint16_t app_otaGetFirmwareState(void)
{
    uint16_t read_eeprom_data[1] = {0};
    memset(read_eeprom_data, 0x00, 1);
    EEPROM_ReadWords(APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START, read_eeprom_data, 1);

    uint8_t u16Tou8_data[2] = {0,0};
    convertUint16ToUint8(read_eeprom_data, u16Tou8_data, 1, 2);

    return combineUint8ArrayToUint16(u16Tou8_data);
}

/** App固件 状态、 版本字符串、大小、crc校验和测试代码

// 保存App固件版本
char tempStr[] = "0.0.1234567890-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-261234567890-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-261234567890-11-12-13-14-15-16-17-18-19-20-21-22-23-24-25-26";
app_otaSetVersion(tempStr,strlen(tempStr));
SEGGER_RTT_printf(0, "ota set app of version :%s %d\r\n", tempStr, strlen(tempStr)); 
// 获取App固件版本
char tempReadStr[254] = { 0 };
memset(tempReadStr,0x00,254);
app_otaGetVersion(tempReadStr);
SEGGER_RTT_printf(0, "ota get app of version: %s--->%d \r\n", tempReadStr,strlen(tempReadStr)); 


// 保存App固件大小 0xFFFFFFA1
app_otaSetFirmwareSize(0xAAFFFFBB);
// 获取App固件大小
SEGGER_RTT_printf(0, "app_otaGetFirmwareSize:%08x \r\n",app_otaGetFirmwareSize());  
HAL_Delay(1);


// 保存App固件crc校验和
app_otaSetFirmwareCrcChecksum(0x66FFFF88);
// 获取App固件crc校验和
SEGGER_RTT_printf(0, "app_otaGetFirmwareCrcChecksum:%08x \r\n",app_otaGetFirmwareCrcChecksum());  
HAL_Delay(1);
    

// 保存App固件状态
app_otaSetFirmwareState(0x1122);
// 获取App固件状态
SEGGER_RTT_printf(0, "app_otaGetFirmwareState:%04x\r\n", app_otaGetFirmwareState());  


**/


