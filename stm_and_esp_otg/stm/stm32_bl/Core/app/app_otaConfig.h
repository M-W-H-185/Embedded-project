#ifndef __APP_OTACONFIG_H
#define __APP_OTACONFIG_H

#include <stdlib.h>
#include "stdint.h"
#include "string.h"

// App固件状态
#define APP_FIRMWARE_STATE_EEPROM_ADDRESS_START 0
#define APP_FIRMWARE_STATE_EEPROM_ADDRESS_END   1

// App固件大小
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_START   2
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_END     5

// App固件crc校验和
#define APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START      6
#define APP_APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_END    9

// App版本地址长度
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_START 10
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_END 11

// App版本地址字符串
#define APP_VERSION_STR_EEPROM_ADDRESS_START 12
#define APP_VERSION_STR_EEPROM_ADDRESS_END 253

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


// 保存App固件状态
void app_otaSetFirmwareState(uint16_t firmwareState);
// 获取App固件状态
uint32_t app_otaGetFirmwareState(void);

#endif



