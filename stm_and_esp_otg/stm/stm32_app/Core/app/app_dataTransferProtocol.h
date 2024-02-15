#ifndef __APP_DATA_TRANSFER_PROTOCOL_H_
#define __APP_DATA_TRANSFER_PROTOCOL_H_
#include <stdlib.h>
#include "stdint.h"
#include "string.h"

// 两个帧头
#define UART_DATA_HEAD_1 0xED
#define UART_DATA_HEAD_2 0x90
// 两个帧头


void USAR1_UART_IDLECallback(uint8_t *dataBuffer,uint16_t dataBuffer_size);
void protocol_dev_version_data(void);


#endif
