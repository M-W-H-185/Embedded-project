/**
* 数据协议传输层
*
**/
#include "app_dataTransferProtocol.h"
#include "main.h"
#include "app_otaConfig.h"
#include "bsp_uart1.h"
#include "utility_common.h"
#include "SEGGER_RTT.h"


void USAR1_UART_IDLECallback(uint8_t *data_buffer, uint16_t data_buffer_length)
{

    SEGGER_RTT_printf(0, "Receive Data(length = %d): ",data_buffer_length);
    //HAL_UART_Transmit(&huart1,uart1_rx_buff,data_length,USRT1_RX_BUFF_SIZE);            //测试函数：将接收到的数据打印出去
    // 校验帧头
    if(data_buffer[0] != UART_DATA_HEAD_1 || data_buffer[1] != UART_DATA_HEAD_2)
    {
        SEGGER_RTT_printf(0,"DataProtocol read head error!\r\n");
        uart1_empty_data_buffer();
        return;
    }
    // 帧头1 + 帧头2 + cmd_id + isack + size_d + size_h + ....dataBlock
    // 命令帧
    uint8_t  cmd_id = data_buffer[2];
    // 是否需要应答
    uint8_t  isack  = data_buffer[3];
    // 数据块的长度
    uint16_t data_block_size = ((data_buffer)[4] << 8) | (data_buffer)[5];  
    // 数据块
    uint8_t *data_block = &(data_buffer[6]);
    // 串口接收后解析的crc modbus 校验和
    uint16_t uart_calculateCRC = calculateCRC(data_buffer,data_buffer_length - 2);
    // 串口接收到的crc modbus校验和
    uint16_t uart_read_crc = COMBINE_BYTES_TO_UINT16(data_buffer[data_buffer_length-1],data_buffer[data_buffer_length-2]);
    // 到了这里代表串口接收到的数据是完整的
    
    SEGGER_RTT_printf(0,"DataProtocol Success!\r\n");

    SEGGER_RTT_printf(0, 
    "analysis: cmd_id:%02x isack:%02x data_block_size:%d  uart_calculateCRC:%d uart_read_crc:%d uartCRC_L:%02x uartCRC_H:%02x\r\n",
        cmd_id,isack, data_block_size, uart_calculateCRC, uart_read_crc, data_buffer[data_buffer_length-2], data_buffer[data_buffer_length-1]
    );
    // crc modbus 校验和
    if(uart_calculateCRC != uart_read_crc)
    {
        SEGGER_RTT_printf(0,"DataProtocol read crc modbus error!\r\n");
        uart1_empty_data_buffer();
        return;
    }
    // 到了这里整条数据就是完整正确的
    if(cmd_id == 0x02)
    {
        protocol_dev_version_data();
        SEGGER_RTT_printf(0,"DataProtocol cmd_id 0x02 App to BootLoader...\r\n");
        app_otaSetFirmwareState(0x0003);
        NVIC_SystemReset();

    }
    uart1_empty_data_buffer();
}

// uart_data 传入一个空指针进来
// cmd_id 命令id
// isack 是否应答
// data 数据区
// data_length 数据区的长度
int uart_send_cmdid_data_handle(char *uart_rx_data, uint8_t cmd_id, uint8_t isack, char* data, int data_length)
{
    
    int uart_rx_data_length = data_length + 8;
    

    uart_rx_data[0] = UART_DATA_HEAD_1;
    uart_rx_data[1] = UART_DATA_HEAD_2;
    uart_rx_data[2] = cmd_id;
    uart_rx_data[3] = isack;

    uart_rx_data[4] =  (data_length >> 8) & 0xFF; // 数据长度 高位
    uart_rx_data[5] = data_length & 0xFF;       // 数据长度 低位

    char *uart_data_ = &uart_rx_data[6];
    memcpy(uart_data_, data, data_length * sizeof(char));

    // -2 是要把校验位剪了
    uint16_t crc = calculateCRC((uint8_t *)uart_rx_data, uart_rx_data_length - 2);
    uart_rx_data[uart_rx_data_length - 2] = crc & 0xFF;       // 校验和 低位
    uart_rx_data[uart_rx_data_length - 1] = (crc >> 8) & 0xFF; // 校验和 高位

    return uart_rx_data_length;
}

// 发送设备版本信息
void protocol_dev_version_data(void)
{
    // 获取版本信息
    char version_str[255] = { 0 };
    uint8_t version_length = app_otaGetVersion(version_str);
    if(version_length == 0xff){return;};
    
    // 拼接命令发送出去
    char data[300] = {0};
    uint16_t data_size = version_length + 2;
    data[0] = 0x02;             // 代表App区域
    data[1] = version_length;   // 字符串的长度
    memcpy((uint8_t*)&data[2] , version_str, version_length);
    
    char rx_data[200] = {0};
    int uart_rx_data_length = uart_send_cmdid_data_handle(rx_data, 0x01, 0x02, data, data_size);
    uart1_send_data((uint8_t *)rx_data, uart_rx_data_length);
}
