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
    if(USART1 == huart1.Instance)                                   //�ж��Ƿ��Ǵ���1�����˴�Ӧд(huart->Instance == USART1)
    {
        if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
        {
            //��������жϱ�־�������һֱ���Ͻ����жϣ�
            __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     
            //ֹͣ����DMA����
            HAL_UART_DMAStop(&huart1);      
            //������յ������ݳ��� buff_size - dmaʣ�µĿռ�
            data_buffer_length  = DATA_UART1_DATA_BUFF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx); 
            
            //�����û��жϴ�����
            USAR1_UART_IDLECallback(data_buffer, data_buffer_length); 
            
            //������ʼDMA���� ÿ��DATA_UART1_DATA_BUFF_SIZE�ֽ�����
            uart1_empty_data_buffer();

        }
    }
}

void uart1_send_data(uint8_t *data_buffer, uint16_t data_length)
{
    HAL_UART_Transmit(&huart1, data_buffer, data_length, 100);            //���Ժ����������յ������ݴ�ӡ��ȥ
}

// ��մ��� 1 ���ݻ�����
void uart1_empty_data_buffer(void)
{
    memset(data_buffer, 0, DATA_UART1_DATA_BUFF_SIZE);     
    HAL_UART_Receive_DMA(&huart1, (uint8_t*)data_buffer, DATA_UART1_DATA_BUFF_SIZE); 
    data_buffer_length = 0;
}


