#include "bsp_uart1.h"
#include <stdlib.h>
#include "stdint.h"
#include "string.h"
#include "main.h"
#include "app_dataTransferProtocol.h"

uint8_t data_buffer[DATA_UART1_DATA_BUFF_SIZE]; 
uint8_t data_buffer_length  = 0;



void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{
    if(USART1 == huart1.Instance)                                   //判断是否是串口1（！此处应写(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            //清楚空闲中断标志（否则会一直不断进入中断）
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     
            //停止本次DMA传输
            HAL_UART_DMAStop(&huart1);      
            //计算接收到的数据长度 buff_size - dma剩下的空间
            data_buffer_length  = DATA_UART1_DATA_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); 
            
            //调用用户中断处理函数
            USAR1_UART_IDLECallback(data_buffer, data_buffer_length); 
            
            //重启开始DMA传输 每次DATA_UART1_DATA_BUFF_SIZE字节数据
            uart1_empty_data_buffer();

        }
    }
}

void uart1_send_data(uint8_t *data_buffer, uint16_t data_length)
{
    HAL_UART_Transmit(&huart1, data_buffer, data_length, 100);            //测试函数：将接收到的数据打印出去
}

// 清空串口 1 数据缓冲区
void uart1_empty_data_buffer(void)
{
    memset(data_buffer, 0, DATA_UART1_DATA_BUFF_SIZE);     
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)data_buffer, DATA_UART1_DATA_BUFF_SIZE); 
    data_buffer_length = 0;
}


