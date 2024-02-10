#ifndef __APP_OTACONFIG_H
#define __APP_OTACONFIG_H

#include <stdlib.h>
#include "stdint.h"
#include "string.h"

// 固件大小
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_START   0
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_END     3

// 固件crc校验和
#define APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START  4
#define APP_APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_END    7

// 版本地址长度
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_START 8
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_END 9

// 版本地址字符串
#define APP_VERSION_STR_EEPROM_ADDRESS_START 10
#define APP_VERSION_STR_EEPROM_ADDRESS_END 259

// 下一个eeprom起始地址就是260

// 保存ota App版本
void app_otaSetVersion(char *version_str, uint8_t version_length);
// 获取ota App版本
uint8_t app_otaGetVersion(char *version_str);

// 保存App固件大小
void app_otaSetFirmwareSize(uint32_t firmwareSize);
// 获取App固件大小
uint32_t app_otaGetFirmwareSize(void);


// 保存App固件crc校验和
void app_otaSetFirmwareCrcChecksum(uint32_t firmwareCrcChecksum);
// 获取App固件crc校验和
uint32_t app_otaGetFirmwareCrcChecksum(void);



#endif



