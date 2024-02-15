#ifndef __BSP_UART1_H_
#define __BSP_UART1_H_
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "main.h"
// 数据缓冲区大小
#define DATA_UART1_DATA_BUFF_SIZE 1024

void USER_UART_IRQHandler(UART_HandleTypeDef *huart);

void uart1_send_data(uint8_t *data_buffer, uint16_t data_length);

void uart1_empty_data_buffer(void);
#endif
