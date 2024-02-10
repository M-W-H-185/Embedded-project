#ifndef __APP_OTACONFIG_H
#define __APP_OTACONFIG_H

#include <stdlib.h>
#include "stdint.h"
#include "string.h"

// �̼���С
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_START   0
#define APP_FIRMWARE_SIZE_EEPROM_ADDRESS_END     3

// �̼�crcУ���
#define APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_START  4
#define APP_APP_FIRMWARE_CRC_CHECKSUM_EEPROM_ADDRESS_END    7

// �汾��ַ����
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_START 8
#define APP_VERSION_STR_LENGTH_EEPROM_ADDRESS_END 9

// �汾��ַ�ַ���
#define APP_VERSION_STR_EEPROM_ADDRESS_START 10
#define APP_VERSION_STR_EEPROM_ADDRESS_END 259

// ��һ��eeprom��ʼ��ַ����260

// ����ota App�汾
void app_otaSetVersion(char *version_str, uint8_t version_length);
// ��ȡota App�汾
uint8_t app_otaGetVersion(char *version_str);

// ����App�̼���С
void app_otaSetFirmwareSize(uint32_t firmwareSize);
// ��ȡApp�̼���С
uint32_t app_otaGetFirmwareSize(void);


// ����App�̼�crcУ���
void app_otaSetFirmwareCrcChecksum(uint32_t firmwareCrcChecksum);
// ��ȡApp�̼�crcУ���
uint32_t app_otaGetFirmwareCrcChecksum(void);



#endif



